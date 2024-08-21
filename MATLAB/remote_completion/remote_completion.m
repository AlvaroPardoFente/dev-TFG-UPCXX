%% Add common functions to the path

clearvars;
addpath("../include/")

%% ----------------------PRINTING----------------------

do_print = false;

%% Import and process data

int_size = 4;

figure_position = [680 458 480 240];

data = struct();
clean_data = struct();

base_path = "../../results/remote_completion/";

data.global_var_1N_2n = importtable(base_path + "upcxx_rpc_global_var_1N_2n");
data.global_var_2N_4n = importtable(base_path + "upcxx_rpc_global_var_2N_4n");
data.global_var_4N_8n = importtable(base_path + "upcxx_rpc_global_var_4N_8n");
data.global_var_6N_12n = importtable(base_path + "upcxx_rpc_global_var_6N_12n");
data.global_var_8N_16n = importtable(base_path + "upcxx_rpc_global_var_8N_16n");
data.global_var_10N_20n = importtable(base_path + "upcxx_rpc_global_var_10N_20n");
data.global_var_12N_24n = importtable(base_path + "upcxx_rpc_global_var_12N_24n");

data.dist_object_1N_2n = importtable(base_path + "upcxx_rpc_dist_object_1N_2n");
data.dist_object_2N_4n = importtable(base_path + "upcxx_rpc_dist_object_2N_4n");
data.dist_object_4N_8n = importtable(base_path + "upcxx_rpc_dist_object_4N_8n");
data.dist_object_6N_12n = importtable(base_path + "upcxx_rpc_dist_object_6N_12n");
data.dist_object_8N_16n = importtable(base_path + "upcxx_rpc_dist_object_8N_16n");
data.dist_object_10N_20n = importtable(base_path + "upcxx_rpc_dist_object_10N_20n");
data.dist_object_12N_24n = importtable(base_path + "upcxx_rpc_dist_object_12N_24n");

unique_sizes = unique(data.global_var_1N_2n.Size);
unique_sizes_bytes = unique_sizes;
unique_sizes_categorical = categorical(unique_sizes);

num_processes = [2; 4; 8; 12; 16; 20; 24];

% Get field names
fields = fieldnames(data);

% Apply function to each field
for i = 1:numel(fields)
    clean_data.(fields{i}) = rmopersize(data.(fields{i}));
end

%% Bandwidth means

bandwidth_data = struct();
bandwidth_mean = struct();
data_mean = struct();

for i = 1:numel(fields)
    bandwidth_data.(fields{i}) = (clean_data.(fields{i}).Size .* int_size) ./ clean_data.(fields{i}).Time;
    bandwidth_mean.(fields{i}) = arrayfun(@(size) ...
        mean(bandwidth_data.(fields{i})(clean_data.(fields{i}).Size == size)), ...
        unique_sizes);
    data_mean.(fields{i}) = arrayfun(@(size) ...
        mean(clean_data.(fields{i}).Time(clean_data.(fields{i}).Size == size)), ...
        unique_sizes);
end

%% Plot utilities

markers = ["o"; "+"; "x";"square"; "diamond"];
formatted_fields = regexprep(fields, "_", "\\_");
size_tick_labels = {'1', '4', '16', '64', '256', '1K', '4K', '16K'};

%% SURF TEST
% 
% % Initialize empty structs for MPI and UPC++
% global_var_bandwidth_mean = struct();
% upcxx_bandwidth_mean = struct();
% 
% % Separate MPI and UPC++ data
% for i = 1:numel(fields)
%     if contains(fields{i}, 'global_var')
%         global_var_bandwidth_mean.(fields{i}) = bandwidth_mean.(fields{i});
%     elseif contains(fields{i}, 'upcxx')
%         upcxx_bandwidth_mean.(fields{i}) = bandwidth_mean.(fields{i});
%     end
% end
% 
% % Initialize matrices
% [X, Y] = meshgrid(num_processes, unique_sizes_bytes);
% 
% Z_global_var = zeros(length(unique_sizes_bytes), length(num_processes));
% Z_upcxx = zeros(length(unique_sizes_bytes), length(num_processes));
% 
% % Fill Z matrices with MPI data
% %global_var_fields = fieldnames(global_var_bandwidth_mean);
% %for i = 1:length(global_var_fields)
% %    Z_global_var(:, i) = global_var_bandwidth_mean.(global_var_fields{i});
% %end
% 
% % Fill Z matrices with UPC++ data
% upcxx_fields = fieldnames(upcxx_bandwidth_mean);
% for i = 1:length(upcxx_fields)
%     Z_upcxx(:, i) = upcxx_bandwidth_mean.(upcxx_fields{i});
% end
% 
% % Plot both MPI and UPC++ data together
% figure;
% 
% % Plot MPI data
% %surf(X, Y, Z_global_var, 'FaceAlpha', 0.5, 'EdgeColor', 'none', 'FaceColor', [1 0 0]);
% %hold on;
% 
% % Plot UPC++ data
% surf(X, Y, Z_upcxx, 'FaceAlpha', 1);
% 
% % Customize plot appearance
% xlabel('Number of Processes');
% ylabel('Size (Bytes)');
% zlabel('Bandwidth (B/s)');
% if (~do_print)
%     title('3D Surface Plot of Mean Bandwidth (MPI and UPCXX)');
% end
% % legend('UPCXX');
% colormap jet;
% colorbar;
% 
% % Set logarithmic scales
% set(gca, 'YScale', 'log');
% set(gca, 'ZScale', 'log');
% 
% if (do_print)
%     print("upcxx_remote_completion_3D", "-dpng");
% end
% 
% hold off;

%% 2 processes

sizes_figure_2n = figure("Position", figure_position);

%nexttile
plot(unique_sizes_bytes, bandwidth_mean.global_var_1N_2n, "--o")
hold on
plot(unique_sizes_bytes, bandwidth_mean.dist_object_1N_2n, "--x")
set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
min_bandwidth = min(min([bandwidth_mean.global_var_1N_2n, bandwidth_mean.dist_object_1N_2n]));
max_bandwidth = max(max([bandwidth_mean.global_var_1N_2n, bandwidth_mean.dist_object_1N_2n]));
ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])

legend("global\_var", "dist\_object", "Location","southeast");
xlabel('Iterations');
ylabel('Bandwidth (B/s)');
if (~do_print)
    title('Mean bandwidth per size on 2 processes (1 node)');
end
grid on;

if (do_print)
    print("remote_completion_1N_2n", "-dpng");
end

%% 4 processes

sizes_figure_4n = figure("Position", figure_position);

%nexttile
plot(unique_sizes_bytes, bandwidth_mean.global_var_2N_4n, "--o")
hold on
plot(unique_sizes_bytes, bandwidth_mean.dist_object_2N_4n, "--x")
set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
min_bandwidth = min(min([bandwidth_mean.global_var_2N_4n, bandwidth_mean.dist_object_2N_4n]));
max_bandwidth = max(max([bandwidth_mean.global_var_2N_4n, bandwidth_mean.dist_object_2N_4n]));
ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();

xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend("global\_var", "dist\_object", "Location","southeast");
xlabel('Iterations');
ylabel('Bandwidth (B/s)');
if (~do_print)
    title('Mean bandwidth per size on 4 processes (2 nodes)');
end
grid on;

if (do_print)
    print("remote_completion_2N_4n", "-dpng");
end

difference_2N = abs(bandwidth_mean.global_var_2N_4n ./ bandwidth_mean.dist_object_2N_4n);
dispmaxdiff('[2N, global\_var]', difference_2N, size_tick_labels);

difference_2N_upcxx = abs(bandwidth_mean.dist_object_2N_4n ./ bandwidth_mean.global_var_2N_4n);
dispmaxdiff('[2N, upcxx]', difference_2N_upcxx, size_tick_labels);

%% 8 processes

sizes_figure_8n = figure("Position", figure_position);

%nexttile
plot(unique_sizes_bytes, bandwidth_mean.global_var_4N_8n, "--o")
hold on
plot(unique_sizes_bytes, bandwidth_mean.dist_object_4N_8n, "--x")
set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
min_bandwidth = min(min([bandwidth_mean.global_var_4N_8n, bandwidth_mean.dist_object_4N_8n]));
max_bandwidth = max(max([bandwidth_mean.global_var_4N_8n, bandwidth_mean.dist_object_4N_8n]));
ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();

xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend("global\_var", "dist\_object", "Location","southeast");
xlabel('Iterations');
ylabel('Bandwidth (B/s)');
if (~do_print)
    title('Mean bandwidth per size on 8 processes (4 nodes)');
end
grid on;

if (do_print)
    print("remote_completion_4N_8n", "-dpng");
end

difference_4N = abs(bandwidth_mean.global_var_4N_8n ./ bandwidth_mean.dist_object_4N_8n);
dispmaxdiff('[4N, global\_var]', difference_4N, size_tick_labels);

%% 64B: all processes

% Extract bandwidth for 64 bytes
size_64_idx = unique_sizes_bytes == 64;

% Initialize arrays for MPI and UPCXX
global_var_bandwidth_64B = zeros(length(num_processes), 1);
dist_object_bandwidth_64B = zeros(length(num_processes), 1);

% Extract data for each process count
for i = 1:length(num_processes)
    global_var_field = ['global_var_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    dist_object_field = ['dist_object_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    
    global_var_bandwidth_64B(i) = bandwidth_mean.(global_var_field)(size_64_idx);
    dist_object_bandwidth_64B(i) = bandwidth_mean.(dist_object_field)(size_64_idx);
end

%% Plot bandwidth for 64 bytes against number of processes

figure('Position', figure_position);

plot(num_processes, global_var_bandwidth_64B, '--o', 'DisplayName', 'global_var')
hold on
plot(num_processes, dist_object_bandwidth_64B, '--x', 'DisplayName', 'upcxx')
set(gca, 'YScale', 'log')

remove_m_ticks();
xlim([min(num_processes) max(num_processes)]);
legend("global\_var", "dist\_object", 'Location', 'northeast');
xlabel('Number of Processes');
ylabel('Bandwidth (B/s)');

ax = gca;
ax.XTick = num_processes;
ax.XTickLabel = num_processes;

if (~do_print)
    title('Bandwidth for 64 iterations against number of processes');
end
grid on;

if (do_print)
    print("remote_completion_64I_all", "-dpng");
end

difference_64B = abs(global_var_bandwidth_64B ./ dist_object_bandwidth_64B);
dispmaxdiff('[64B, global\_var]', difference_64B, num_processes);

%% Extract bandwidth for 16 KB
size_16KB_idx = unique_sizes_bytes == 4 * 1024;

% Initialize arrays for MPI and UPCXX
global_var_bandwidth_16KB = zeros(length(num_processes), 1);
dist_object_bandwidth_16KB = zeros(length(num_processes), 1);

% Extract data for each process count
for i = 1:length(num_processes)
    global_var_field = ['global_var_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    dist_object_field = ['dist_object_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    
    global_var_bandwidth_16KB(i) = bandwidth_mean.(global_var_field)(size_16KB_idx);
    dist_object_bandwidth_16KB(i) = bandwidth_mean.(dist_object_field)(size_16KB_idx);
end

%% Plot bandwidth for 16 KB against number of processes
figure('Position', figure_position);

plot(num_processes, global_var_bandwidth_16KB, '--o', 'DisplayName', 'global_var')
hold on
plot(num_processes, dist_object_bandwidth_16KB, '--x', 'DisplayName', 'upcxx')
set(gca, 'YScale', 'log')
remove_m_ticks();
xlim([min(num_processes) max(num_processes)])
legend("global\_var", "dist\_object", 'Location', 'northeast');
xlabel('Number of Processes');
ylabel('Bandwidth (B/s)');

ax = gca;
ax.XTick = num_processes;
ax.XTickLabel = num_processes;

if (~do_print)
    title('Bandwidth for 4 K iterations against number of processes');
end
grid on;

if (do_print)
    print("remote_completion_4KI_all", "-dpng");
end

difference_16KB = (global_var_bandwidth_16KB ./ dist_object_bandwidth_16KB);
dispmaxdiff('[16KB, global\_var]', difference_16KB, num_processes)

difference_dist_object_16KB = (dist_object_bandwidth_16KB ./ global_var_bandwidth_16KB);
dispmaxdiff('[16KB, upcxx]', difference_dist_object_16KB, num_processes);

% %% 8 nodes: Times in different sizes
% 
% global_var_bandwidth = global_var_8n_clean_data.Size ./ global_var_8n_clean_data.Time;
% global_var_mean_bandwidth = arrayfun(@(size) mean(global_var_bandwidth(global_var_8n_clean_data.Size == size)), unique_sizes);
% dist_object_bandwidth = dist_object_8n_clean_data.Size ./ dist_object_8n_clean_data.Time;
% dist_object_mean_bandwidth = arrayfun(@(size) mean(dist_object_bandwidth(dist_object_8n_clean_data.Size == size)), unique_sizes);
% 
% global_var_mean_times = arrayfun(@(size) mean(global_var_8n_clean_data.Time(global_var_8n_clean_data.Size == size)), unique_sizes);
% dist_object_mean_times = arrayfun(@(size) mean(dist_object_8n_clean_data.Time(dist_object_8n_clean_data.Size == size)), unique_sizes);
% 
% sizes_figure = figure("Position", figure_position);
% 
% %tiledlayout("flow");
% 
% %nexttile
% %loglog(unique_sizes, global_var_mean_times, "-o", "DisplayName", "global_var mean")
% %hold on
% %loglog(unique_sizes, dist_object_mean_times, "-o", "DisplayName", "upcxx mean")
% 
% %legend("global_var", "dist\_object");
% %xlabel('Iterations');
% %ylabel('Time (s)');
% %title('Mean times without outliers');
% %grid on;
% 
% %nexttile
% plot(unique_sizes, global_var_mean_bandwidth, "--o")
% hold on
% plot(unique_sizes, dist_object_mean_bandwidth, "--o")
% set(gca, "XScale", "log")
% xlim([min(unique_sizes) max(unique_sizes)])
% legend("global_var", "dist\_object");
% xlabel('Iterations');
% ylabel('Bandwidth (4B/s)');
% title('Mean bandwidth per size on 8 processes');
% grid on;
% 
% % Calculate the directional relative difference for each size
% directional_diff = (global_var_mean_bandwidth - dist_object_mean_bandwidth) ./ ((global_var_mean_bandwidth + dist_object_mean_bandwidth) / 2) * 100;
% 
% % Calculate the average percentage difference
% average_directional_diff = mean(directional_diff);
% 
% % Display the average percentage difference
% disp(['Average Percentage Difference in Bandwidth: ', num2str(average_directional_diff), '%']);
% 
% %nexttile
% %bar(unique_sizes_categorical, [global_var_mean_bandwidth, dist_object_mean_bandwidth])
% %legend("global_var", "dist\_object");
% %xlabel('Iterations');
% %ylabel('Bandwidth (4B/s)');
% %title('Mean bandwidth per size');
% %grid on;
% 
% % print("remote_completion_sizes", "-dpng")
% 
% %% 4MB: Times in different number of nodes
% 
% num_nodes = [2, 4, 6, 8, 10, 12];
% size = 4 * 1024 * 1024;
% 
% global_var_2n_4M = global_var_2n_clean_data(global_var_2n_clean_data.Size == size, :);
% global_var_4n_4M = global_var_4n_clean_data(global_var_4n_clean_data.Size == size, :);
% global_var_6n_4M = global_var_6n_clean_data(global_var_6n_clean_data.Size == size, :);
% global_var_8n_4M = global_var_8n_clean_data(global_var_8n_clean_data.Size == size, :);
% global_var_10n_4M = global_var_10n_clean_data(global_var_10n_clean_data.Size == size, :);
% global_var_12n_4M = global_var_12n_clean_data(global_var_12n_clean_data.Size == size, :);
% 
% dist_object_2n_4M = dist_object_2n_clean_data(dist_object_2n_clean_data.Size == size, :);
% dist_object_4n_4M = dist_object_4n_clean_data(dist_object_4n_clean_data.Size == size, :);
% dist_object_6n_4M = dist_object_6n_clean_data(dist_object_6n_clean_data.Size == size, :);
% dist_object_8n_4M = dist_object_8n_clean_data(dist_object_8n_clean_data.Size == size, :);
% dist_object_10n_4M = dist_object_10n_clean_data(dist_object_10n_clean_data.Size == size, :);
% dist_object_12n_4M = dist_object_12n_clean_data(dist_object_12n_clean_data.Size == size, :);
% 
% global_var_mean_times_4M = [
%     mean(global_var_2n_4M.Time);
%     mean(global_var_4n_4M.Time);
%     mean(global_var_6n_4M.Time);
%     mean(global_var_8n_4M.Time);
%     mean(global_var_10n_4M.Time);
%     mean(global_var_12n_4M.Time)
% ];
% 
% dist_object_mean_times_4M = [
%     mean(dist_object_2n_4M.Time);
%     mean(dist_object_4n_4M.Time);
%     mean(dist_object_6n_4M.Time);
%     mean(dist_object_8n_4M.Time);
%     mean(dist_object_10n_4M.Time);
%     mean(dist_object_12n_4M.Time)
% ];
% 
% % Calculate the directional relative difference for each node count
% directional_diff = (global_var_mean_times_4M - dist_object_mean_times_4M) ./ ((global_var_mean_times_4M + dist_object_mean_times_4M) / 2) * 100;
% 
% % Calculate the average percentage difference
% average_directional_diff = mean(directional_diff);
% 
% % Display the average percentage difference
% disp(['Average Percentage Difference in Mean Times: ', num2str(average_directional_diff), '%']);
% 
% nodes_figure = figure("Position", figure_position);
% bar(num_nodes, [global_var_mean_times_4M, dist_object_mean_times_4M]);
% hold on;
% xlabel('Number of Nodes');
% ylabel('Mean Time (s)');
% title('Mean Times for Size = 4M Across Different Node Counts');
% legend("global_var", "dist\_object", "Location","northwest");
% grid on;
% 
% % print("remote_completion_nodes", "-dpng")
