%% Utility function to get min and max across multiple fields
function [min_val, max_val] = get_min_max(bandwidth_mean, fields)
    min_val = inf;
    max_val = -inf;
    for i = 1:numel(fields)
        field_name = fields{i};
        min_val = min(min_val, min(bandwidth_mean.(field_name)));
        max_val = max(max_val, max(bandwidth_mean.(field_name)));
    end
end