function control = auv_simple_controller_core(state, targetSpeed, targetHeading, massSurge, Xu, Xuu, maxSpeed, maxYawRate, speedGain, kpYaw, kdYaw)
% AUV_SIMPLE_CONTROLLER_CORE computes the same heading/speed control as the
% Phase 1 function, but without a params struct or toolbox helper.

psi = state(3);
currentSpeed = state(4);
currentYawRate = state(6);
currentHeading = wrap_to_pi_local(psi);
headingError = wrap_to_pi_local(targetHeading - currentHeading);

speedError = targetSpeed - currentSpeed;
feedforwardAccel = (Xu * targetSpeed + Xuu * abs(targetSpeed) * targetSpeed) / massSurge;

uCmd = max(-maxSpeed, min(maxSpeed, speedGain * speedError + feedforwardAccel));
yawRateCmd = max(-maxYawRate, min(maxYawRate, kpYaw * headingError - kdYaw * currentYawRate));

control = [uCmd; yawRateCmd];
end
