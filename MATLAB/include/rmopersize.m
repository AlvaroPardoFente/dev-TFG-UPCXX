function [output_table] = rmopersize(input_table)
%ROMPERSIZE Removes time outliers grouped by size
%   A table with Size and Time columns is passed. The function divides
%   the data based on Size and removes outliers based on Time
cleaned_data = table([], [], [], [], 'VariableNames', {'Size', 'Timepoint', 'Index', 'Time'});

% Get unique sizes and timepoints
unique_sizes = unique(input_table.Size);
unique_timepoints = unique(input_table.Timepoint);

% Loop over each size group
for i = 1:length(unique_sizes)
    size_group = unique_sizes(i);
    % Extract the group data
    group_data = input_table(input_table.Size == size_group, :);
    
    % Remove outliers from the 'Time' column
    [group_data_cleaned, TF] = rmoutliers(group_data, 'DataVariables', 'Time');
    
    % Append the cleaned group data to the cleaned_data table
    cleaned_data = [cleaned_data; group_data_cleaned];
end

output_table = cleaned_data;

end