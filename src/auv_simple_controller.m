function control = auv_simple_controller(state, targetSpeed, targetHeading, params)
% AUV_SIMPLE_CONTROLLER converts speed and heading references into surge thrust
% and yaw-rate commands with simple feedback.
%
% Controller structure:
%   uCmd = Kp_speed * (v_ref - u) + feedforward_drag_compensation
%   yawRateCmd = Kp_yaw * headingError - Kd_yaw * yawRate
%
% The drag-compensation term is needed because the model has substantial
% quadratic damping; without it, the simple proportional speed loop cannot hold a
% commanded nonzero speed in steady state.

psi = state(3);
currentSpeed = state(4);
currentYawRate = state(6);
currentHeading = wrapToPi(psi);
headingError = wrapToPi(targetHeading - currentHeading);

speedError = targetSpeed - currentSpeed;
feedforwardAccel = (params.damping.Xu * targetSpeed + ...
    params.damping.Xuu * abs(targetSpeed) * targetSpeed) / params.dynamics.massSurge;

uCmd = max(-params.control.maxSpeed, min(params.control.maxSpeed, ...
    params.control.speedGain * speedError + feedforwardAccel));

yawRateCmd = max(-params.control.maxYawRate, min(params.control.maxYawRate, ...
    params.control.kpYaw * headingError - params.control.kdYaw * currentYawRate));

control = [uCmd; yawRateCmd];
end
