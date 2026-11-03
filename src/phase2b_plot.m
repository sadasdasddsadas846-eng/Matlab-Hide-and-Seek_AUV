function phase2b_plot()
% PHASE2B_PLOT plots the Simulink-generated trajectories for the two-agent
% Phase 2B model. The script rebuilds the model if the results do not exist yet.

if ~exist('phase2b_results', 'var')
    build_phase2b_model();
end

results = evalin('base', 'phase2b_results');
A = results.trajA;
B = results.trajB;

figure('Color', 'w', 'Position', [100 100 900 600]);
plot(A(:, 1), A(:, 2), 'b', 'LineWidth', 2); hold on;
plot(B(:, 1), B(:, 2), 'r', 'LineWidth', 2);
plot(A(1, 1), A(1, 2), 'bo', 'MarkerSize', 8, 'LineWidth', 2);
plot(B(1, 1), B(1, 2), 'ro', 'MarkerSize', 8, 'LineWidth', 2);
legend('Agent A (stealth)', 'Agent B (seeker)', 'Location', 'best');
grid on;
axis equal;
xlabel('x (m)');
ylabel('y (m)');
title('Phase 2B Simulink two-agent trajectories');
end
