classdef test_phase4_agent_b < matlab.unittest.TestCase
    methods (Test)
        function pursuesWhenTargetDetected(testCase)
            config = phase4_config();
            stateB = [8.0; 6.0; deg2rad(90); 0.0; 0.0; 0.0];
            targetPos = [12.0; 6.0];
            [mode, command, detected, ~] = phase4_agent_b_policy(stateB, targetPos, config, 0.0);
            testCase.verifyEqual(mode, 'PURSUE');
            testCase.verifyTrue(detected);
            testCase.verifyGreaterThan(command(1), 0.0);
        end

        function capturesWhenClose(testCase)
            config = phase4_config();
            stateB = [8.0; 6.0; 0.0; 0.0; 0.0; 0.0];
            targetPos = [8.8; 6.0];
            [mode, command, detected, ~] = phase4_agent_b_policy(stateB, targetPos, config, 0.0);
            testCase.verifyEqual(mode, 'CAPTURE');
            testCase.verifyGreaterThanOrEqual(command(1), 0.0);
        end

        function searchesWhenTargetOutsideBeam(testCase)
            config = phase4_config();
            stateB = [8.0; 6.0; deg2rad(90)];
            targetPos = [30.0; 30.0];
            [mode, command, detected, ~] = phase4_agent_b_policy(stateB, targetPos, config, 0.0);
            testCase.verifyEqual(mode, 'SEARCH');
            testCase.verifyFalse(detected);
            testCase.verifyGreaterThan(command(1), 0.0);
        end
    end
end
