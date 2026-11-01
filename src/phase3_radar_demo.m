function phase3_radar_demo()
% PHASE3_RADAR_DEMO draws the planar arena and a rotating radar sector.
% A single fixed target is shown so the beam motion can be inspected visually.

params = radar_params();
radarPos = [12; 12];
targetPos = [20; 18];

figure('Color', 'w', 'Position', [100 100 900 700]);
axis equal;
axis([0 params.arenaSize 0 params.arenaSize]);
grid on;
hold on;

% Arena boundary.
rectangle('Position', [0 0 params.arenaSize params.arenaSize], ...
    'EdgeColor', 'k', 'LineWidth', 1.5, 'FaceColor', 'none');

sampleTimes = [0, 1.0, params.scanPeriod / 4, params.scanPeriod / 2, ...
               3 * params.scanPeriod / 4, params.scanPeriod];

plot(targetPos(1), targetPos(2), 'kx', 'MarkerSize', 12, 'LineWidth', 2);
plot(radarPos(1), radarPos(2), 'bo', 'MarkerSize', 10, 'LineWidth', 2);

for i = 1:length(sampleTimes)
    t = sampleTimes(i);
    theta = radar_beam_angle(t, params);
    halfWidth = params.beamwidth / 2;
    left = theta - halfWidth;
    right = theta + halfWidth;

    thetaGrid = linspace(left, right, 60);
    x = radarPos(1) + params.maxRange * cos(thetaGrid);
    y = radarPos(2) + params.maxRange * sin(thetaGrid);

    [detected, distance, bearingErrorDeg, beamDirDeg, targetBearingDeg] = radar_detects(params, radarPos, targetPos, t);

    patchColor = [0.7 0.8 1.0];
    if detected
        patchColor = [1.0 0.75 0.75];
    end

    patch([radarPos(1), x, radarPos(1)], [radarPos(2), y, radarPos(2)], ...
        patchColor, 'FaceAlpha', 0.25, 'EdgeColor', 'b', 'LineWidth', 1.0);

    beamTip = radarPos + params.maxRange * [cos(theta); sin(theta)];
    plot([radarPos(1), beamTip(1)], [radarPos(2), beamTip(2)], 'b', 'LineWidth', 1.5);

    labelPos = beamTip + 0.9 * [cos(theta); sin(theta)];
    text(labelPos(1), labelPos(2), sprintf('t=%.1fs\ndet=%d\nerr=%.1fdeg', ...
        t, detected, bearingErrorDeg), 'Color', 'k', 'FontSize', 9, ...
        'HorizontalAlignment', 'left', 'VerticalAlignment', 'middle');

    if detected
        plot(targetPos(1), targetPos(2), 'ro', 'MarkerSize', 10, 'LineWidth', 2);
    end
end

xlabel('x (m)');
ylabel('y (m)');
title('Phase 3 radar sector demo');
legend('Target', 'Radar', 'Location', 'northwest');
end
