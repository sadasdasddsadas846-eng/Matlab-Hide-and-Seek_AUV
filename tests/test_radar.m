classdef test_radar < matlab.unittest.TestCase
    % TEST_RADAR verifies the fixed-frequency radar geometry used by the seeker.

    methods (TestClassSetup)
        function addSrcToPath(testCase)
            repoRoot = fileparts(fileparts(mfilename('fullpath')));
            testCase.applyFixture(matlab.unittest.fixtures.PathFixture(fullfile(repoRoot, 'src')));
        end
    end

    methods (Test)
        function insideBeam(testCase)
            params = radar_params();
            radarPos = [10; 0];
            targetPos = [18; 2];

            [detected, distance, bearingErrorDeg, beamDirectionDeg, targetBearingDeg] = ...
                radar_detects(params, radarPos, targetPos, 0);

            testCase.verifyTrue(detected);
            testCase.verifyEqual(distance, hypot(8, 2), 'AbsTol', 1e-9);
            testCase.verifyLessThanOrEqual(bearingErrorDeg, params.beamwidthDeg / 2 + 1e-9);
            testCase.verifyEqual(beamDirectionDeg, 0, 'AbsTol', 1e-9);
            testCase.verifyEqual(targetBearingDeg, atand(2 / 8), 'AbsTol', 1e-9);
        end

        function outsideRange(testCase)
            params = radar_params();
            radarPos = [0; 0];
            targetPos = [20; 0];

            [detected, distance, bearingErrorDeg] = radar_detects(params, radarPos, targetPos, 0);

            testCase.verifyFalse(detected);
            testCase.verifyEqual(distance, 20, 'AbsTol', 1e-9);
            testCase.verifyEqual(bearingErrorDeg, 0, 'AbsTol', 1e-9);
        end

        function outsideBeamwidth(testCase)
            params = radar_params();
            radarPos = [0; 0];
            targetPos = [10; 10];

            [detected, distance, bearingErrorDeg] = radar_detects(params, radarPos, targetPos, 0);

            testCase.verifyFalse(detected);
            testCase.verifyGreaterThan(bearingErrorDeg, params.beamwidthDeg / 2);
        end

        function exactlyOnBoundary(testCase)
            params = radar_params();
            radarPos = [0; 0];
            targetPos = [10 * cosd(15), 10 * sind(15)];

            [detected, distance, bearingErrorDeg] = radar_detects(params, radarPos, targetPos, 0);

            testCase.verifyTrue(detected);
            testCase.verifyEqual(distance, 10, 'AbsTol', 1e-9);
            testCase.verifyEqual(bearingErrorDeg, 15, 'AbsTol', 1e-9);
        end

        function angleWrapNear180(testCase)
            params = radar_params();
            params.theta0 = deg2rad(-179);
            radarPos = [0; 0];
            targetPos = [10 * cosd(179), 10 * sind(179)];

            [detected, distance, bearingErrorDeg, beamDirectionDeg, targetBearingDeg] = ...
                radar_detects(params, radarPos, targetPos, 0);

            testCase.verifyTrue(detected);
            testCase.verifyEqual(bearingErrorDeg, 2, 'AbsTol', 1e-9);
            testCase.verifyEqual(beamDirectionDeg, -179, 'AbsTol', 1e-9);
            testCase.verifyEqual(targetBearingDeg, 179, 'AbsTol', 1e-9);
        end

        function samePositionAsRadar(testCase)
            params = radar_params();
            radarPos = [0; 0];
            targetPos = [0; 0];

            [detected, distance, bearingErrorDeg] = radar_detects(params, radarPos, targetPos, 0);

            testCase.verifyTrue(detected);
            testCase.verifyEqual(distance, 0, 'AbsTol', 1e-9);
            testCase.verifyEqual(bearingErrorDeg, 0, 'AbsTol', 1e-9);
        end

        function oneRotationAfterScanPeriod(testCase)
            params = radar_params();
            theta = radar_beam_angle(params.scanPeriod, params);
            testCase.verifyLessThanOrEqual(abs(mod(theta, 2*pi)), 1e-9);
        end
    end
end
