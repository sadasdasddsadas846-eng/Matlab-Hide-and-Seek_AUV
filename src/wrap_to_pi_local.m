function angle = wrap_to_pi_local(angle)
% WRAP_TO_PI_LOCAL emulates wrapToPi in a MATLAB Function block without
% calling a toolbox helper. It keeps the principal angle in (-pi, pi] and
% preserves the boundary convention for exact multiples of pi.

if isempty(angle)
    angle = [];
    return;
end

if isvector(angle)
    angle = angle(:);
end

wrapped = mod(angle + pi, 2 * pi) - pi;
idx = abs(mod(angle, pi)) < 1e-12;
wrapped(idx) = 0;
wrapped(idx & (angle < 0)) = -pi;
wrapped(idx & (angle > 0)) = pi;

angle = wrapped;
if isvector(angle)
    angle = angle(:);
end
end
