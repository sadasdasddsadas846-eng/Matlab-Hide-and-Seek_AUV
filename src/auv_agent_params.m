function params = auv_agent_params(agentId)
% AUV_AGENT_PARAMS returns the parameter struct for one vehicle agent.
%
% Inputs:
%   agentId  - numeric 1 or 2, or string 'A' or 'B'
%
% Outputs:
%   params   - parameter struct with the simulation base values plus agent-
%              specific initial conditions and target commands.

if ischar(agentId)
    agentId = upper(agentId);
    if strcmp(agentId, 'A')
        agentId = 1;
    elseif strcmp(agentId, 'B')
        agentId = 2;
    else
        error('Unsupported agent ID: %s', agentId);
    end
end

switch agentId
    case 1
        params = auv_params();
        params.agentName = 'A';
        params.initialState = [0; 0; 0; 0; 0; 0];
        params.targetSpeed = 1.0;       % m/s
        params.targetHeading = deg2rad(0); % rad
    case 2
        params = auv_params();
        params.agentName = 'B';
        params.initialState = [8; 6; deg2rad(90); 0; 0; 0];
        params.targetSpeed = 0.8;       % m/s
        params.targetHeading = deg2rad(180); % rad
    otherwise
        error('Agent ID must be 1 (A) or 2 (B).');
end
end
