function dState = auv_planar_dynamics_core(state, uCmd, yawRateCmd, massSurge, massSway, yawInertia, Xu, Xuu, Yv, Yvv, yawLinear, yawQuadratic)
% AUV_PLANAR_DYNAMICS_CORE computes the planar 3-DOF dynamics using plain
% numeric scalar inputs with no struct or toolbox dependency.

x = state(1);
y = state(2);
psi = state(3);
u = state(4);
v = state(5);
r = state(6);

surgeForce = uCmd * massSurge;
surgeForce = surgeForce - Xu * u;
surgeForce = surgeForce - Xuu * abs(u) * u;

swayForce = -Yv * v;
swayForce = swayForce - Yvv * abs(v) * v;

yawTorque = yawRateCmd * yawInertia;
yawTorque = yawTorque - yawLinear * r;
yawTorque = yawTorque - yawQuadratic * abs(r) * r;

uDot = surgeForce / massSurge;
vDot = swayForce / massSway;
rDot = yawTorque / yawInertia;

bodyToWorld = [cos(psi), -sin(psi); sin(psi), cos(psi)];
bodyVel = [u; v];
worldVel = bodyToWorld * bodyVel;

xDot = worldVel(1);
yDot = worldVel(2);
psiDot = r;

dState = [xDot; yDot; psiDot; uDot; vDot; rDot];
end
