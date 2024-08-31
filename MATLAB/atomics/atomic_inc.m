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

base_path = "../../results/atomics/";

data.mpi_1N_2n = importtable(base_path + "mpi_atomic_inc_1N_2n");
data.mpi_2N_4n = importtable(base_path + "mpi_atomic_inc_2N_4n");
data.mpi_4N_8n = importtable(base_path + "mpi_atomic_inc_4N_8n");
data.mpi_6N_12n = importtable(base_path + "mpi_atomic_inc_6N_12n");
data.mpi_8N_16n = importtable(base_path + "mpi_atomic_inc_8N_16n");
data.mpi_10N_20n = importtable(base_path + "mpi_atomic_inc_10N_20n");
data.mpi_12N_24n = importtable(base_path + "mpi_atomic_inc_12N_24n");

data.upcxx_1N_2n = importtable(base_path + "upcxx_atomic_inc_1N_2n");
data.upcxx_2N_4n = importtable(base_path + "upcxx_atomic_inc_2N_4n");
data.upcxx_4N_8n = importtable(base_path + "upcxx_atomic_inc_4N_8n");
data.upcxx_6N_12n = importtable(base_path + "upcxx_atomic_inc_6N_12n");
data.upcxx_8N_16n = importtable(base_path + "upcxx_atomic_inc_8N_16n");
data.upcxx_10N_20n = importtable(base_path + "upcxx_atomic_inc_10N_20n");
data.upcxx_12N_24n = importtable(base_path + "upcxx_atomic_inc_12N_24n");

unique_sizes = unique(data.mpi_1N_2n.Size);
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

data_mean = struct();

for i = 1:numel(fields)
    data_mean.(fields{i}) = arrayfun(@(size) ...
        mean(clean_data.(fields{i}).Time(clean_data.(fields{i}).Size == size)), ...
        unique_sizes);
end

%% Plot utilities

markers = ["o"; "+"; "x";"square"; "diamond"];
formatted_fields = regexprep(fields, "_", "\\_");
size_tick_labels = {'1', '2', '8', '32', '128', '512', '2K', '8K'};

%% SURF TEST

% Initialize empty structs for MPI and UPC++
mpi_bandwidth_mean = struct();
upcxx_bandwidth_mean = struct();

% Separate MPI and UPC++ data
for i = 1:numel(fields)
    if contains(fields{i}, 'mpi')
        mpi_bandwidth_mean.(fields{i}) = data_mean.(fields{i});
    elseif contains(fields{i}, 'upcxx')
        upcxx_bandwidth_mean.(fields{i}) = data_mean.(fields{i});
    end
end

% Initialize matrices
[X, Y] = meshgrid(num_processes, unique_sizes_bytes);

Z_mpi = zeros(length(unique_sizes_bytes), length(num_processes));
Z_upcxx = zeros(length(unique_sizes_bytes), length(num_processes));

% Fill Z matrices with MPI data
%mpi_fields = fieldnames(mpi_bandwidth_mean);
%for i = 1:length(mpi_fields)
%    Z_mpi(:, i) = mpi_bandwidth_mean.(mpi_fields{i});
%end

% Fill Z matrices with UPC++ data
upcxx_fields = fieldnames(upcxx_bandwidth_mean);
for i = 1:length(upcxx_fields)
    Z_upcxx(:, i) = upcxx_bandwidth_mean.(upcxx_fields{i});
end

% Plot both MPI and UPC++ data together
figure;

% Plot MPI data
%surf(X, Y, Z_mpi, 'FaceAlpha', 0.5, 'EdgeColor', 'none', 'FaceColor', [1 0 0]);
%hold on;

% Plot UPC++ data
surf(X, Y, Z_upcxx, 'FaceAlpha', 1);

% Customize plot appearance
xlabel('Number of Processes');
ylabel('Size(Bytes)');
zlabel('Time (s)');
if (~do_print)
    title('3D Surface Plot of Mean Time (MPI and UPCXX)');
end
% legend('UPCXX');
colormap jet;
colorbar;

% Set logarithmic scales
set(gca, 'YScale', 'log');
set(gca, 'ZScale', 'log');

if (do_print)
    print("upcxx_atomic_inc_3D", "-dpng");
end

hold off;

%% 2 processes

sizes_figure_2n = figure("Position", figure_position);

%nexttile
plot(unique_sizes_bytes, data_mean.mpi_1N_2n, "--o")
hold on
plot(unique_sizes_bytes, data_mean.upcxx_1N_2n, "--x")
set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
min_bandwidth = min(min([data_mean.mpi_1N_2n, data_mean.upcxx_1N_2n]));
max_bandwidth = max(max([data_mean.mpi_1N_2n, data_mean.upcxx_1N_2n]));
ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])

legend("mpi", "upcxx", "Location","southeast");
xlabel('Iterations');
ylabel('Time (s)');
if (~do_print)
    title('Mean time per size on 2 processes (1 node)');
end
grid on;

if (do_print)
    print("atomic_inc_1N_2n", "-dpng");
end

%% 4 processes

sizes_figure_4n = figure("Position", figure_position);

%nexttile
plot(unique_sizes_bytes, data_mean.mpi_2N_4n, "--o")
hold on
plot(unique_sizes_bytes, data_mean.upcxx_2N_4n, "--x")
set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
min_bandwidth = min(min([data_mean.mpi_2N_4n, data_mean.upcxx_2N_4n]));
max_bandwidth = max(max([data_mean.mpi_2N_4n, data_mean.upcxx_2N_4n]));
ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();

xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend("mpi", "upcxx", "Location","east");
xlabel('Iterations');
ylabel('Time (s)');
if (~do_print)
    title('Mean time per size on 4 processes (2 nodes)');
end
grid on;

if (do_print)
    print("atomic_inc_2N_4n", "-dpng");
end

difference_2N = abs(data_mean.mpi_2N_4n ./ data_mean.upcxx_2N_4n);
dispmaxdiff('[2N, mpi]', difference_2N, size_tick_labels);

difference_2N_upcxx = abs(data_mean.upcxx_2N_4n ./ data_mean.mpi_2N_4n);
dispmaxdiff('[2N, upcxx]', difference_2N_upcxx, size_tick_labels);

%% 8 processes

sizes_figure_8n = figure("Position", figure_position);

%nexttile
plot(unique_sizes_bytes, data_mean.mpi_4N_8n, "--o")
hold on
plot(unique_sizes_bytes, data_mean.upcxx_4N_8n, "--x")
set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
min_bandwidth = min(min([data_mean.mpi_4N_8n, data_mean.upcxx_4N_8n]));
max_bandwidth = max(max([data_mean.mpi_4N_8n, data_mean.upcxx_4N_8n]));
ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();

xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend("mpi", "upcxx", "Location","southeast");
xlabel('Iterations');
ylabel('Time (s)');
if (~do_print)
    title('Mean time per size on 8 processes (4 nodes)');
end
grid on;

if (do_print)
    print("atomic_inc_4N_8n", "-dpng");
end

difference_4N = abs(data_mean.mpi_4N_8n ./ data_mean.upcxx_4N_8n);
dispmaxdiff('[4N, mpi]', difference_4N, size_tick_labels);

%% 8 Iterations: all processes

% Extract bandwidth for 64 bytes
size_8_idx = unique_sizes_bytes == 8;

% Initialize arrays for MPI and UPCXX
mpi_time_8I = zeros(length(num_processes), 1);
upcxx_time_8I = zeros(length(num_processes), 1);

% Extract data for each process count
for i = 1:length(num_processes)
    mpi_field = ['mpi_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    upcxx_field = ['upcxx_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    
    mpi_time_8I(i) = data_mean.(mpi_field)(size_8_idx);
    upcxx_time_8I(i) = data_mean.(upcxx_field)(size_8_idx);
end

%% Plot bandwidth for 64 bytes against number of processes

figure('Position', figure_position);

plot(num_processes, mpi_time_8I, '--o', 'DisplayName', 'mpi')
hold on
plot(num_processes, upcxx_time_8I, '--x', 'DisplayName', 'upcxx')
set(gca, 'YScale', 'log')

remove_m_ticks();
xlim([min(num_processes) max(num_processes)]);
legend('Location', 'northeast');
xlabel('Number of Processes');
ylabel('Time (s)');

ax = gca;
ax.XTick = num_processes;
ax.XTickLabel = num_processes;

if (~do_print)
    title('Bandwidth for 8 iterations against number of processes');
end
grid on;

if (do_print)
    print("atomic_inc_8I_all", "-dpng");
end

difference_8I = abs(mpi_time_8I ./ upcxx_time_8I);
dispmaxdiff('[8I, mpi]', difference_8I, num_processes);

difference_8I_upcxx = abs(upcxx_time_8I ./ mpi_time_8I);
dispmaxdiff('[8I, upcxx]', difference_8I_upcxx, num_processes);

%% Extract bandwidth for 2K iterations
size_2KI_idx = unique_sizes_bytes == 2 * 1024;

% Initialize arrays for MPI and UPCXX
mpi_time_2KI = zeros(length(num_processes), 1);
upcxx_time_2KI = zeros(length(num_processes), 1);

% Extract data for each process count
for i = 1:length(num_processes)
    mpi_field = ['mpi_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    upcxx_field = ['upcxx_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    
    mpi_time_2KI(i) = data_mean.(mpi_field)(size_2KI_idx);
    upcxx_time_2KI(i) = data_mean.(upcxx_field)(size_2KI_idx);
end

%% Plot bandwidth for 2K iterations against number of processes
figure('Position', figure_position);

plot(num_processes, mpi_time_2KI, '--o', 'DisplayName', 'mpi')
hold on
plot(num_processes, upcxx_time_2KI, '--x', 'DisplayName', 'upcxx')
set(gca, 'YScale', 'log')
remove_m_ticks();
xlim([min(num_processes) max(num_processes)])
legend('Location', 'northeast');
xlabel('Number of Processes');
ylabel('Time (s)');

ax = gca;
ax.XTick = num_processes;
ax.XTickLabel = num_processes;

if (~do_print)
    title('Bandwidth for 2K iterations against number of processes');
end
grid on;

if (do_print)
    print("atomic_inc_2KI_all", "-dpng");
end

difference_16KB = (mpi_time_2KI ./ upcxx_time_2KI);
dispmaxdiff('[2KI, mpi]', difference_16KB, num_processes)

difference_upcxx_16KB = (upcxx_time_2KI ./ mpi_time_2KI);
dispmaxdiff('[2KI, upcxx]', difference_upcxx_16KB, num_processes);

% %% 8 nodes: Times in different sizes
% 
% mpi_bandwidth = mpi_8n_clean_data.Size ./ mpi_8n_clean_data.Time;
% mpi_mean_bandwidth = arrayfun(@(size) mean(mpi_bandwidth(mpi_8n_clean_data.Size == size)), unique_sizes);
% upcxx_bandwidth = upcxx_8n_clean_data.Size ./ upcxx_8n_clean_data.Time;
% upcxx_mean_bandwidth = arrayfun(@(size) mean(upcxx_bandwidth(upcxx_8n_clean_data.Size == size)), unique_sizes);
% 
% mpi_mean_times = arrayfun(@(size) mean(mpi_8n_clean_data.Time(mpi_8n_clean_data.Size == size)), unique_sizes);
% upcxx_mean_times = arrayfun(@(size) mean(upcxx_8n_clean_data.Time(upcxx_8n_clean_data.Size == size)), unique_sizes);
% 
% sizes_figure = figure("Position", figure_position);
% 
% %tiledlayout("flow");
% 
% %nexttile
% %loglog(unique_sizes, mpi_mean_times, "-o", "DisplayName", "mpi mean")
% %hold on
% %loglog(unique_sizes, upcxx_mean_times, "-o", "DisplayName", "upcxx mean")
% 
% %legend("mpi", "upcxx");
% %xlabel('Iterations');
% %ylabel('Time (s)');
% %title('Mean times without outliers');
% %grid on;
% 
% %nexttile
% plot(unique_sizes, mpi_mean_bandwidth, "--o")
% hold on
% plot(unique_sizes, upcxx_mean_bandwidth, "--o")
% set(gca, "XScale", "log")
% xlim([min(unique_sizes) max(unique_sizes)])
% legend("mpi", "upcxx");
% xlabel('Iterations');
% ylabel('Bandwidth (4B/s)');
% title('Mean time per size on 8 processes');
% grid on;
% 
% % Calculate the directional relative difference for each size
% directional_diff = (mpi_mean_bandwidth - upcxx_mean_bandwidth) ./ ((mpi_mean_bandwidth + upcxx_mean_bandwidth) / 2) * 100;
% 
% % Calculate the average percentage difference
% average_directional_diff = mean(directional_diff);
% 
% % Display the average percentage difference
% disp(['Average Percentage Difference in Bandwidth: ', num2str(average_directional_diff), '%']);
% 
% %nexttile
% %bar(unique_sizes_categorical, [mpi_mean_bandwidth, upcxx_mean_bandwidth])
% %legend("mpi", "upcxx");
% %xlabel('Iterations');
% %ylabel('Bandwidth (4B/s)');
% %title('Mean time per size');
% %grid on;
% 
% % print("atomic_inc_sizes", "-dpng")
% 
% %% 4MB: Times in different number of nodes
% 
% num_nodes = [2, 4, 6, 8, 10, 12];
% size = 4 * 1024 * 1024;
% 
% mpi_2n_4M = mpi_2n_clean_data(mpi_2n_clean_data.Size == size, :);
% mpi_4n_4M = mpi_4n_clean_data(mpi_4n_clean_data.Size == size, :);
% mpi_6n_4M = mpi_6n_clean_data(mpi_6n_clean_data.Size == size, :);
% mpi_8n_4M = mpi_8n_clean_data(mpi_8n_clean_data.Size == size, :);
% mpi_10n_4M = mpi_10n_clean_data(mpi_10n_clean_data.Size == size, :);
% mpi_12n_4M = mpi_12n_clean_data(mpi_12n_clean_data.Size == size, :);
% 
% upcxx_2n_4M = upcxx_2n_clean_data(upcxx_2n_clean_data.Size == size, :);
% upcxx_4n_4M = upcxx_4n_clean_data(upcxx_4n_clean_data.Size == size, :);
% upcxx_6n_4M = upcxx_6n_clean_data(upcxx_6n_clean_data.Size == size, :);
% upcxx_8n_4M = upcxx_8n_clean_data(upcxx_8n_clean_data.Size == size, :);
% upcxx_10n_4M = upcxx_10n_clean_data(upcxx_10n_clean_data.Size == size, :);
% upcxx_12n_4M = upcxx_12n_clean_data(upcxx_12n_clean_data.Size == size, :);
% 
% mpi_mean_times_4M = [
%     mean(mpi_2n_4M.Time);
%     mean(mpi_4n_4M.Time);
%     mean(mpi_6n_4M.Time);
%     mean(mpi_8n_4M.Time);
%     mean(mpi_10n_4M.Time);
%     mean(mpi_12n_4M.Time)
% ];
% 
% upcxx_mean_times_4M = [
%     mean(upcxx_2n_4M.Time);
%     mean(upcxx_4n_4M.Time);
%     mean(upcxx_6n_4M.Time);
%     mean(upcxx_8n_4M.Time);
%     mean(upcxx_10n_4M.Time);
%     mean(upcxx_12n_4M.Time)
% ];
% 
% % Calculate the directional relative difference for each node count
% directional_diff = (mpi_mean_times_4M - upcxx_mean_times_4M) ./ ((mpi_mean_times_4M + upcxx_mean_times_4M) / 2) * 100;
% 
% % Calculate the average percentage difference
% average_directional_diff = mean(directional_diff);
% 
% % Display the average percentage difference
% disp(['Average Percentage Difference in Mean Times: ', num2str(average_directional_diff), '%']);
% 
% nodes_figure = figure("Position", figure_position);
% bar(num_nodes, [mpi_mean_times_4M, upcxx_mean_times_4M]);
% hold on;
% xlabel('Number of Nodes');
% ylabel('Mean Time (s)');
% title('Mean Times for Size = 4M Across Different Node Counts');
% legend("mpi", "upcxx", "Location","northwest");
% grid on;
% 
% % print("atomic_inc_nodes", "-dpng")