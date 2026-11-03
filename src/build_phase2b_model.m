function [results, mdl] = build_phase2b_model()
% BUILD_PHASE2B_MODEL creates a lightweight two-agent Simulink wrapper that
% reuses the validated Phase 1 equations via auv_step_numeric with a single
% MATLAB Function block per agent.

repoRoot = fileparts(fileparts(mfilename('fullpath')));
addpath(fullfile(repoRoot, 'src'));
modelsDir = fullfile(repoRoot, 'models');
if ~exist(modelsDir, 'dir')
    mkdir(modelsDir);
end

mdl = 'phase2b_two_agents';
if bdIsLoaded(mdl)
    close_system(mdl, 0);
end

new_system(mdl);

paramsA = auv_agent_params('A');
paramsB = auv_agent_params('B');

paramsA.initialState = [0; 0; 0; 0; 0; 0];
paramsA.targetSpeed = 1.0;
paramsA.targetHeading = deg2rad(0);

paramsB.initialState = [8; 6; deg2rad(90); 0; 0; 0];
paramsB.targetSpeed = 0.8;
paramsB.targetHeading = deg2rad(90);

vecA = [paramsA.targetSpeed; paramsA.targetHeading; ...
        paramsA.dynamics.massSurge; ...
        paramsA.dynamics.massSway; ...
        paramsA.dynamics.yawInertia; ...
        paramsA.damping.Xu; ...
        paramsA.damping.Xuu; ...
        paramsA.damping.Yv; ...
        paramsA.damping.Yvv; ...
        paramsA.damping.YawLinear; ...
        paramsA.damping.YawQuadratic; ...
        paramsA.control.maxSpeed; ...
        paramsA.control.maxYawRate; ...
        paramsA.control.speedGain; ...
        paramsA.control.kpYaw; ...
        paramsA.control.kdYaw];

vecB = [paramsB.targetSpeed; paramsB.targetHeading; ...
        paramsB.dynamics.massSurge; ...
        paramsB.dynamics.massSway; ...
        paramsB.dynamics.yawInertia; ...
        paramsB.damping.Xu; ...
        paramsB.damping.Xuu; ...
        paramsB.damping.Yv; ...
        paramsB.damping.Yvv; ...
        paramsB.damping.YawLinear; ...
        paramsB.damping.YawQuadratic; ...
        paramsB.control.maxSpeed; ...
        paramsB.control.maxYawRate; ...
        paramsB.control.speedGain; ...
        paramsB.control.kpYaw; ...
        paramsB.control.kdYaw];

% Agent A blocks.
add_block('simulink/Sources/Constant', [mdl '/A_u']);
set_param([mdl '/A_u'], 'Value', mat2str(vecA));
add_block('simulink/Continuous/Integrator', [mdl '/A_xInt']);
set_param([mdl '/A_xInt'], 'InitialCondition', mat2str(paramsA.initialState));
add_block('simulink/User-Defined Functions/MATLAB Function', [mdl '/A_f']);
add_block('simulink/Sinks/To Workspace', [mdl '/A_log']);
set_param([mdl '/A_log'], 'VariableName', 'phase2b_stateA');
set_param([mdl '/A_log'], 'SaveFormat', 'Array');
codeA = sprintf('function xdot = fcn(x, u)\nxdot = auv_step_numeric(x, u);\nend');
set_mlfcn_script([mdl '/A_f'], codeA);

% Agent B blocks.
add_block('simulink/Sources/Constant', [mdl '/B_u']);
set_param([mdl '/B_u'], 'Value', mat2str(vecB));
add_block('simulink/Continuous/Integrator', [mdl '/B_xInt']);
set_param([mdl '/B_xInt'], 'InitialCondition', mat2str(paramsB.initialState));
add_block('simulink/User-Defined Functions/MATLAB Function', [mdl '/B_f']);
add_block('simulink/Sinks/To Workspace', [mdl '/B_log']);
set_param([mdl '/B_log'], 'VariableName', 'phase2b_stateB');
set_param([mdl '/B_log'], 'SaveFormat', 'Array');
codeB = sprintf('function xdot = fcn(x, u)\nxdot = auv_step_numeric(x, u);\nend');
set_mlfcn_script([mdl '/B_f'], codeB);

set_param(mdl, 'Solver', 'ode4');
set_param(mdl, 'FixedStep', '0.05');
set_param(mdl, 'StopTime', '20');

add_line(mdl, 'A_u/1', 'A_f/2');
add_line(mdl, 'A_xInt/1', 'A_f/1');
add_line(mdl, 'A_f/1', 'A_xInt/1');
add_line(mdl, 'A_xInt/1', 'A_log/1');

add_line(mdl, 'B_u/1', 'B_f/2');
add_line(mdl, 'B_xInt/1', 'B_f/1');
add_line(mdl, 'B_f/1', 'B_xInt/1');
add_line(mdl, 'B_xInt/1', 'B_log/1');

save_system(mdl, fullfile(modelsDir, mdl));
plain = phase2_two_agents();
refA = [plain.finalA(1), plain.finalA(2), plain.finalA(3)];
refB = [plain.finalB(1), plain.finalB(2), plain.finalB(3)];

out = sim(mdl);
trajA = squeeze(out.get('phase2b_stateA')).';
trajB = squeeze(out.get('phase2b_stateB')).';
trajA = trajA(:, 1:3);
trajB = trajB(:, 1:3);
finalA = trajA(end, :);
finalB = trajB(end, :);
maxDiff = max([abs(finalA(1) - refA(1)), abs(finalA(2) - refA(2)), ...
              abs(finalB(1) - refB(1)), abs(finalB(2) - refB(2))]);

fprintf('SIMULINK vs plain MATLAB check\n');
fprintf('Agent A final: x = %.2f, y = %.2f, psi = %.2f\n', finalA(1), finalA(2), finalA(3));
fprintf('Agent A reference: x = %.2f, y = %.2f, psi = %.2f\n', refA(1), refA(2), refA(3));
fprintf('Agent B final: x = %.2f, y = %.2f, psi = %.2f\n', finalB(1), finalB(2), finalB(3));
fprintf('Agent B reference: x = %.2f, y = %.2f, psi = %.2f\n', refB(1), refB(2), refB(3));
fprintf('Max position difference: %.4f m\n', maxDiff);

results = struct('trajA', trajA, 'trajB', trajB, 'finalA', finalA, 'finalB', finalB, ...
    'refA', refA, 'refB', refB, 'maxDiff', maxDiff);
assignin('base', 'phase2b_results', results);
end
