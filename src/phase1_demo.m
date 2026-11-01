% PHASE1_DEMO
% Run this file in MATLAB to inspect the simplified planar 3-DOF model.
% The script plots a step response for speed and heading.
%
% Step command used in this demo:
%   desired speed = 1.0 m/s
%   desired heading = 30 deg = pi/6 rad
%   step applied at t = 5.0 s
%
% This is intentionally a baseline model for Phase 1 only. It does not yet
% include the hide-and-seek state machine or radar sector detection.

params = auv_params();

desiredSpeed = 1.0;              % m/s
preferredHeading = deg2rad(30);  % 30 degrees
stepTime = 5.0;                  % s
[t, state, controlHistory] = auv_planar_step_response(params, desiredSpeed, preferredHeading);

figure('Color', 'w', 'Position', [100, 100, 900, 700]);
subplot(2, 1, 1);
plot(t, state(:, 4), 'LineWidth', 2);
hold on;
plot(t, ones(size(t)) * desiredSpeed, '--', 'LineWidth', 1.5, 'Color', [0.5 0.5 0.5]);
axis tight;
grid on;
xlabel('Time (s)');
ylabel('Surge speed u (m/s)');
title('Phase 1 baseline step response: surge speed');
legend('Actual speed', 'Commanded speed', 'Location', 'best');

subplot(2, 1, 2);
headingDeg = rad2deg(state(:, 3));
plot(t, headingDeg, 'LineWidth', 2);
hold on;
plot(t, rad2deg(preferredHeading) * ones(size(t)), '--', ...
    'LineWidth', 1.5, 'Color', [0.5 0.5 0.5]);
axis tight;
grid on;
xlabel('Time (s)');
ylabel('Heading psi (deg)');
title('Phase 1 baseline step response: heading');
legend('Actual heading', 'Commanded heading', 'Location', 'best');

sgtitle('Simplified 3-DOF planar AUV baseline model');

% Compute heading-step metrics after the 5 s command change.
headingAfterStep = headingDeg(t >= stepTime);
stepTarget = rad2deg(preferredHeading);
peakHeading = max(headingAfterStep);
overshootPct = max(0, (peakHeading - stepTarget) / stepTarget * 100);
settlingWindow = abs(headingAfterStep - stepTarget) <= 2.0;
settlingIndex = find(settlingWindow, 1, 'first');
if isempty(settlingIndex)
    settlingTime = NaN;
else
    settlingTime = t(find(t >= stepTime, 1, 'first') + settlingIndex - 2) - stepTime;
end

fprintf('Step response complete.\n');
fprintf('Step command: speed = %.1f m/s, heading = %.1f deg, applied at t = %.1f s.\n', ...
    desiredSpeed, stepTarget, stepTime);
fprintf('Final speed (m/s): %.3f\n', state(end, 4));
fprintf('Final heading (deg): %.2f\n', headingDeg(end));
fprintf('Heading overshoot (%%): %.2f\n', overshootPct);
fprintf('Heading settling time after step (s): %.2f\n', settlingTime);
