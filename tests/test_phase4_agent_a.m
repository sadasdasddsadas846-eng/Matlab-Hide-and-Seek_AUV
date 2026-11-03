classdef test_phase4_agent_a < matlab.unittest.TestCase
    methods (Test)
        function reachesGoalWhenClose(testCase)
            config = phase4_config();
            state = [config.goal(1) - 0.1; config.goal(2); 0.0];
            [cmd, mode, detected] = phase4_agent_a_policy(state, [8.0; 6.0], config, 0.0);
            testCase.verifyEqual(mode, 'REACH_GOAL');
            testCase.verifyEqual(cmd(1), 0.0, 'AbsTol', 1e-12);
            testCase.verifyFalse(detected);
        end

        function waitsForGapWhenRadarCoversPath(testCase)
            config = phase4_config();
            state = [10.0; 6.0; 0.0; 0.0; 0.0; 0.0];
            radarPos = [8.0; 6.0];
            [cmd, mode, detected] = phase4_agent_a_policy(state, radarPos, config, 0.0);
            testCase.verifyEqual(mode, 'WAIT_FOR_GAP');
            testCase.verifyTrue(detected);
        end

        function advancesWhenBeamIsClear(testCase)
            config = phase4_config();
            state = [2.0; 2.0; 0.0];
            radarPos = [8.0; 6.0];
            [cmd, mode, detected] = phase4_agent_a_policy(state, radarPos, config, 0.0);
            testCase.verifyEqual(mode, 'ADVANCE');
            testCase.verifyFalse(detected);
            testCase.verifyGreaterThanOrEqual(cmd(1), 0.0);
        end
    end
end
