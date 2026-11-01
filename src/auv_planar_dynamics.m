function dState = auv_planar_dynamics(state, control, params)
% AUV_PLANAR_DYNAMICS returns the planar 3-DOF dynamics.
%
% State vector: [x, y, psi, u, v, r]'
% Inputs: control = [uCmd; yawRateCmd]
%
% This model represents a fixed-depth planar vehicle with surge, sway, and yaw
% motion. It is intentionally simple and transparent so the geometry and
% control logic can be verified before later radar and state-machine phases.

x = state(1);
y = state(2);
psi = state(3);
u = state(4);
v = state(5);
r = state(6);

uCmd = control(1);
yawRateCmd = control(2);

% Surge, sway, and yaw force/torque balance.
surgeForce = uCmd * params.dynamics.massSurge;
surgeForce = surgeForce - params.damping.Xu * u;
surgeForce = surgeForce - params.damping.Xuu * abs(u) * u;

swayForce = -params.damping.Yv * v;
swayForce = swayForce - params.damping.Yvv * abs(v) * v;

yawTorque = yawRateCmd * params.dynamics.yawInertia;
yawTorque = yawTorque - params.damping.YawLinear * r;
yawTorque = yawTorque - params.damping.YawQuadratic * abs(r) * r;

uDot = surgeForce / params.dynamics.massSurge;
vDot = swayForce / params.dynamics.massSway;
rDot = yawTorque / params.dynamics.yawInertia;

% Body-to-world kinematics for planar motion.
bodyToWorld = [cos(psi), -sin(psi); sin(psi), cos(psi)];
bodyVel = [u; v];
worldVel = bodyToWorld * bodyVel;

xDot = worldVel(1);
yDot = worldVel(2);
psiDot = r;

dState = [xDot; yDot; psiDot; uDot; vDot; rDot];
end
