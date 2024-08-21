%% Add common functions to the path

clearvars;
addpath("../include/")

%% ----------------------PRINTING----------------------

do_print = false;

%% Import and process data

int_size = 4;

figure_position = [680 458 480 240];

base_path = "../../results/non-contiguous/";

data = struct();
clean_data = struct();

data.mpi_1N_2n = importfile(base_path + "mpi_vector_1N_2n.csv");
data.upcxx_non_contiguous_1N_2n = importfile(base_path + "upcxx_rput_non_contiguous_1N_2n.csv");
data.upcxx_irregular_1N_2n = importfile(base_path + "upcxx_rput_irregular_1N_2n.csv");
data.upcxx_regular1N_2n = importfile(base_path + "upcxx_rput_regular_1N_2n.csv");
data.upcxx_strided_1N_2n = importfile(base_path + "upcxx_rput_strided_1N_2n.csv");

data.mpi_2N_4n = importfile(base_path + "mpi_vector_2N_4n.csv");
data.upcxx_non_contiguous_2N_4n = importfile(base_path + "upcxx_rput_non_contiguous_2N_4n.csv");
data.upcxx_irregular_2N_4n = importfile(base_path + "upcxx_rput_irregular_2N_4n.csv");
data.upcxx_regular2N_4n = importfile(base_path + "upcxx_rput_regular_2N_4n.csv");
data.upcxx_strided_2N_4n = importfile(base_path + "upcxx_rput_strided_2N_4n.csv");

data.mpi_4N_8n = importfile(base_path + "mpi_vector_4N_8n.csv");
data.upcxx_non_contiguous_4N_8n = importfile(base_path + "upcxx_rput_non_contiguous_4N_8n.csv");
data.upcxx_irregular_4N_8n = importfile(base_path + "upcxx_rput_irregular_4N_8n.csv");
data.upcxx_regular4N_8n = importfile(base_path + "upcxx_rput_regular_4N_8n.csv");
data.upcxx_strided_4N_8n = importfile(base_path + "upcxx_rput_strided_4N_8n.csv");

data.mpi_6N_12n = importfile(base_path + "mpi_vector_6N_12n.csv");
data.upcxx_non_contiguous_6N_12n = importfile(base_path + "upcxx_rput_non_contiguous_6N_12n.csv");
data.upcxx_irregular_6N_12n = importfile(base_path + "upcxx_rput_irregular_6N_12n.csv");
data.upcxx_regular6N_12n = importfile(base_path + "upcxx_rput_regular_6N_12n.csv");
data.upcxx_strided_6N_12n = importfile(base_path + "upcxx_rput_strided_6N_12n.csv");

data.mpi_8N_16n = importfile(base_path + "mpi_vector_8N_16n.csv");
data.upcxx_non_contiguous_8N_16n = importfile(base_path + "upcxx_rput_non_contiguous_8N_16n.csv");
data.upcxx_irregular_8N_16n = importfile(base_path + "upcxx_rput_irregular_8N_16n.csv");
data.upcxx_regular8N_16n = importfile(base_path + "upcxx_rput_regular_8N_16n.csv");
data.upcxx_strided_8N_16n = importfile(base_path + "upcxx_rput_strided_8N_16n.csv");

data.mpi_10N_20n = importfile(base_path + "mpi_vector_10N_20n.csv");
data.upcxx_non_contiguous_10N_20n = importfile(base_path + "upcxx_rput_non_contiguous_10N_20n.csv");
data.upcxx_irregular_10N_20n = importfile(base_path + "upcxx_rput_irregular_10N_20n.csv");
data.upcxx_regular10N_20n = importfile(base_path + "upcxx_rput_regular_10N_20n.csv");
data.upcxx_strided_10N_20n = importfile(base_path + "upcxx_rput_strided_10N_20n.csv");

data.mpi_12N_24n = importfile(base_path + "mpi_vector_12N_24n.csv");
data.upcxx_non_contiguous_12N_24n = importfile(base_path + "upcxx_rput_non_contiguous_12N_24n.csv");
data.upcxx_irregular_12N_24n = importfile(base_path + "upcxx_rput_irregular_12N_24n.csv");
data.upcxx_regular12N_24n = importfile(base_path + "upcxx_rput_regular_12N_24n.csv");
data.upcxx_strided_12N_24n = importfile(base_path + "upcxx_rput_strided_12N_24n.csv");


unique_sizes = unique(data.mpi_1N_2n.Sizeperrank);
unique_sizes_bytes = unique_sizes .* 4;
% TODO repetir las pruebas usando uint32_t

num_processes = [2; 4; 8; 12; 16; 20; 24];

% Get field names
fields = fieldnames(data);
fields_1N_2n = (2:5);
fields_2N_4n = (7:10);
fields_4N_8n = (12:15);
fields_6N_12n = (17:20);
fields_8N_16n = (22:25);
fields_10N_20n = (27:30);
fields_12N_24n = (32:35);

upcxx_all_fields = {fields_1N_2n, fields_2N_4n, fields_4N_8n,...
     fields_6N_12n, fields_8N_16n, fields_10N_20n, fields_12N_24n};

% Apply function to each field
for i = 1:numel(fields)
    clean_data.(fields{i}) = rmopersizeperrank(data.(fields{i}));
end

%% Bandwidth means

bandwidth_data = struct();
bandwidth_mean = struct();
data_mean = struct();

for i = 1:numel(fields)
    bandwidth_data.(fields{i}) = (clean_data.(fields{i}).Sizeperrank .* int_size) ./ clean_data.(fields{i}).Time;
    bandwidth_mean.(fields{i}) = arrayfun(@(size) ...
        mean(bandwidth_data.(fields{i})(clean_data.(fields{i}).Sizeperrank == size)), ...
        unique_sizes);
    data_mean.(fields{i}) = arrayfun(@(size) ...
        mean(clean_data.(fields{i}).Time(clean_data.(fields{i}).Sizeperrank == size)), ...
        unique_sizes);
end

%% Plot utilities

markers = ["o"; "+"; "x";"square"; "diamond"];
formatted_fields = regexprep(fields, "_", "\\_");
size_tick_labels = {'16', '64', '256', '1K', '4K', '16K', '64K', '256K'};
legend_names = {"sequence", "irregular", "regular", "strided"};

%% Plot 2N

sizes_figure_8N = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;
for i = fields_2N_4n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, bandwidth_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

set(gca, "XScale", "log");
set(gca, "YScale", "log");
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
[min_bandwidth, max_bandwidth] = get_min_max(bandwidth_mean, fields(fields_2N_4n));
ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
lgd = legend(legend_names, "Location","northwest");
%lgd.FontSize = 7;
xlabel('Size (Bytes)');
ylabel('Bandwidth (B/s)');
if (~do_print)
    title('Mean bandwidth in N = 2 n = 4');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("non_contiguous_2N_4n", "-dpng");
end

% Biggest diffs in time and bandwidth
% then_mean = data_mean.upcxx_rput_then_8N_8n(2);
% mpi_mean = data_mean.mpi_8N_8n(2);
% 
% then_mean_b = bandwidth_mean.upcxx_rput_then_8N_8n(2);
% mpi_mean_b = bandwidth_mean.mpi_8N_8n(2);
% 
% diff = ((then_mean - mpi_mean) / mpi_mean) * 100;
% disp(['(8N) rput then is ', int2str(diff), '% faster than mpi']);
% diff = ((then_mean_b - mpi_mean_b) / mpi_mean_b) * 100;
% disp(['(8N) (bandwidth) rput then is ', int2str(diff), '% faster than mpi']);

%% Plot 4N

sizes_figure_4N = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;
for i = fields_4N_8n
    marker = markers(marker_i);
    marker_i = marker_i + 1;
    plot_objects(i) = plot(unique_sizes_bytes, bandwidth_mean.(fields{i}), "--", "Marker", marker, 'DisplayName', formatted_fields{i});
end

set(gca, "XScale", "log")
set(gca, "YScale", "log")
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
[min_bandwidth, max_bandwidth] = get_min_max(bandwidth_mean, fields(fields_4N_8n));
ax.YTick = get_ytick_range(min_bandwidth, max_bandwidth);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
lgd = legend(legend_names, "Location","northwest");
%lgd.FontSize = 7;
xlabel('Size (Bytes)');
ylabel('Bandwidth (B/s)');
if (~do_print)
    title('Mean bandwidth in N = 4 n = 8');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("non_contiguous_4N_8n", "-dpng");
end

% Biggest diffs in time and bandwidth
% wait_mean = data_mean.upcxx_rput_wait_4N_8n(1);
% mpi_mean = data_mean.mpi_4N_8n(1);
% 
% wait_mean_b = bandwidth_mean.upcxx_rput_wait_4N_8n(1);
% mpi_mean_b = bandwidth_mean.mpi_4N_8n(1);
% 
% diff = ((wait_mean - mpi_mean) / mpi_mean) * 100;
% disp(['(4N) rput wait is ', int2str(diff), '% faster than mpi']);
% diff = ((wait_mean_b - mpi_mean_b) / mpi_mean_b) * 100;
% disp(['(4N) (bandwidth) rput wait is ', int2str(diff), '% faster than mpi']);
% 
% % Diffs size=1 in fastest 8N and 4N
% then_mean_8N = data_mean.upcxx_rput_then_8N_8n(1);
% wait_mean_4N = data_mean.upcxx_rput_wait_4N_8n(1);
% diff = ((wait_mean_4N - then_mean_8N) / then_mean_8N) * 100;
% disp(['(1) 4N wait is ', int2str(diff), '% faster than 8N then']);

%% Plot best upcxx against mpi 2N 4n

mpi_plot = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

upcxx_best_2N = [];
for i = 1:numel(unique_sizes)
    current_best = 0;
    for j = fields_2N_4n
        current_best = max(bandwidth_mean.(fields{j})(i), current_best);
    end
    upcxx_best_2N = [upcxx_best_2N; current_best];
end

hold on

marker_i = 1;

marker = markers(marker_i);
marker_i = marker_i + 1;
mpi_2N_plot = plot(unique_sizes_bytes, bandwidth_mean.mpi_2N_4n, "--", "Marker", marker, 'DisplayName', formatted_fields{6});

marker = markers(marker_i);
marker_i = marker_i + 1;
upcxx_best_2N_plot = plot(unique_sizes_bytes, upcxx_best_2N, "--", "Marker", marker, 'DisplayName', formatted_fields{10});

set(gca, "XScale", "log");
set(gca, "YScale", "log");
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
ax.YTick = 10.^(0:10);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend_names = {"mpi", "upcxx"};
lgd = legend(legend_names, "Location","southeast");
%lgd.FontSize = 7;
xlabel('Size (Bytes)');
ylabel('Bandwidth (B/s)');
if (~do_print)
    title('Mean bandwidth in best upcxx and mpi 2N');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("non_contiguous_best_2N_4n", "-dpng");
end

difference_2N = abs(bandwidth_mean.mpi_2N_4n ./ upcxx_best_2N);
dispmaxdiff('[2N, mpi]', difference_2N, size_tick_labels)

difference_2N_upcxx = abs(upcxx_best_2N ./ bandwidth_mean.mpi_2N_4n);
dispmaxdiff('[2N, upcxx]', difference_2N_upcxx, size_tick_labels)

%% Plot best upcxx against mpi 4N 8n

mpi_plot = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

upcxx_best_4N = [];
for i = 1:numel(unique_sizes)
    current_best = 0;
    for j = fields_4N_8n
        current_best = max(bandwidth_mean.(fields{j})(i), current_best);
    end
    upcxx_best_4N = [upcxx_best_4N; current_best];
end

hold on

marker_i = 1;

marker = markers(marker_i);
marker_i = marker_i + 1;
mpi_4N_plot = plot(unique_sizes_bytes, bandwidth_mean.mpi_4N_8n, "--", "Marker", marker, 'DisplayName', formatted_fields{6});

marker = markers(marker_i);
marker_i = marker_i + 1;
upcxx_best_4N_plot = plot(unique_sizes_bytes, upcxx_best_4N, "--", "Marker", marker, 'DisplayName', formatted_fields{10});

set(gca, "XScale", "log");
set(gca, "YScale", "log");
ax = gca;
ax.XTick = unique_sizes_bytes;
ax.XTickLabel = size_tick_labels;
ax.YTick = 10.^(0:10);
remove_m_ticks();
xlim([min(unique_sizes_bytes) max(unique_sizes_bytes)])
legend_names = {"mpi", "upcxx"};
lgd = legend(legend_names, "Location","southeast");
%lgd.FontSize = 7;
xlabel('Size (Bytes)');
ylabel('Bandwidth (B/s)');
if (~do_print)
    title('Mean bandwidth in best upcxx and mpi 4N');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("non_contiguous_best_4N_8n", "-dpng");
end

difference_4N = abs(bandwidth_mean.mpi_4N_8n ./ upcxx_best_4N);
dispmaxdiff('[4N, mpi]', difference_4N, size_tick_labels)

difference_4N_upcxx = abs(upcxx_best_4N ./ bandwidth_mean.mpi_4N_8n);
dispmaxdiff('[4N, upcxx]', difference_4N_upcxx, size_tick_labels)

%% 64B: all processes

% Extract bandwidth for 64 bytes
size_64_idx = unique_sizes_bytes == 64;

% Initialize arrays for MPI and UPCXX
mpi_bandwidth_64B = zeros(length(num_processes), 1);
upcxx_bandwidth_64B = zeros(length(num_processes), 1);

% Extract data for each process count
for i = 1:length(num_processes)
    mpi_field = ['mpi_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    mpi_bandwidth_64B(i) = bandwidth_mean.(mpi_field)(size_64_idx);

    current_best = 0;
    for j = upcxx_all_fields{i}
        current_best = max(bandwidth_mean.(fields{j})(size_64_idx), current_best);
    end
    upcxx_bandwidth_64B(i) = current_best;
end

% Plot bandwidth for 64 bytes against number of processes
figure('Position', figure_position);

plot(num_processes, mpi_bandwidth_64B, '--o', 'DisplayName', 'mpi')
hold on
plot(num_processes, upcxx_bandwidth_64B, '--x', 'DisplayName', 'upcxx')
set(gca, 'YScale', 'log')

remove_m_ticks();
xlim([min(num_processes) max(num_processes)]);
legend('Location', 'northeast');
xlabel('Number of Processes');
ylabel('Bandwidth (B/s)');

ax = gca;
ax.XTick = num_processes;
ax.XTickLabel = num_processes;

if (~do_print)
    title('Bandwidth for 64 bytes against number of processes');
end
grid on;

if (do_print)
    print("non_contiguous_64B_all", "-dpng");
end

difference_64B = abs(mpi_bandwidth_64B ./ upcxx_bandwidth_64B);
dispmaxdiff('[64B, mpi]', difference_64B, num_processes)

difference_64B_upcxx = abs(upcxx_bandwidth_64B ./ mpi_bandwidth_64B);
dispmaxdiff('[64B, upcxx]', difference_64B_upcxx, num_processes)

%% 1K: all processes

% Extract bandwidth for 64 bytes
size_1K_idx = unique_sizes_bytes == 1024;

% Initialize arrays for MPI and UPCXX
mpi_bandwidth_1K = zeros(length(num_processes), 1);
upcxx_bandwidth_1K = zeros(length(num_processes), 1);

% Extract data for each process count
for i = 1:length(num_processes)
    mpi_field = ['mpi_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    mpi_bandwidth_1K(i) = bandwidth_mean.(mpi_field)(size_1K_idx);

    current_best = 0;
    for j = upcxx_all_fields{i}
        current_best = max(bandwidth_mean.(fields{j})(size_1K_idx), current_best);
    end
    upcxx_bandwidth_1K(i) = current_best;
end

% Plot bandwidth for 64 bytes against number of processes
figure('Position', figure_position);

plot(num_processes, mpi_bandwidth_1K, '--o', 'DisplayName', 'mpi')
hold on
plot(num_processes, upcxx_bandwidth_1K, '--x', 'DisplayName', 'upcxx')
set(gca, 'YScale', 'log')

remove_m_ticks();
xlim([min(num_processes) max(num_processes)]);
legend('Location', 'northeast');
xlabel('Number of Processes');
ylabel('Bandwidth (B/s)');

ax = gca;
ax.XTick = num_processes;
ax.XTickLabel = num_processes;

if (~do_print)
    title('Bandwidth for 1KB against number of processes');
end
grid on;

if (do_print)
    print("non_contiguous_1KB_all", "-dpng");
end

difference_64B = abs(mpi_bandwidth_1K ./ upcxx_bandwidth_1K);
dispmaxdiff('[64B, mpi]', difference_64B, num_processes)

difference_64B_upcxx = abs(upcxx_bandwidth_1K ./ mpi_bandwidth_1K);
dispmaxdiff('[64B, upcxx]', difference_64B_upcxx, num_processes)

%% Extract bandwidth for 16 KB
size_16KB_idx = unique_sizes_bytes == 16 * 1024;

% Initialize arrays for MPI and UPCXX
mpi_bandwidth_16KB = zeros(length(num_processes), 1);
upcxx_bandwidth_16KB = zeros(length(num_processes), 1);

% Extract data for each process count
for i = 1:length(num_processes)
    mpi_field = ['mpi_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];    
    mpi_bandwidth_16KB(i) = bandwidth_mean.(mpi_field)(size_16KB_idx);

    current_best = 0;
    for j = upcxx_all_fields{i}
        current_best = max(bandwidth_mean.(fields{j})(size_16KB_idx), current_best);
    end
    upcxx_bandwidth_16KB(i) = current_best;
end

% Plot bandwidth for 16 KB against number of processes

figure('Position', figure_position);

plot(num_processes, mpi_bandwidth_16KB, '--o', 'DisplayName', 'mpi')
hold on
plot(num_processes, upcxx_bandwidth_16KB, '--x', 'DisplayName', 'upcxx')
set(gca, 'YScale', 'log')
remove_m_ticks();
xlim([min(num_processes) max(num_processes)])
legend('Location', 'northeast');
xlabel('Number of Processes');
ylabel('Bandwidth (B/s)');

ax = gca;
ax.XTick = num_processes;
ax.XTickLabel = num_processes;

if (~do_print)
    title('Bandwidth for 16 KB against number of processes');
end
grid on;

if (do_print)
    print("non_contiguous_16KB_all", "-dpng");
end

difference_16KB = abs(mpi_bandwidth_16KB ./ upcxx_bandwidth_16KB);
dispmaxdiff('[16KB, mpi]', difference_16KB, num_processes)

difference_16KB_upcxx = abs(upcxx_bandwidth_16KB ./ mpi_bandwidth_16KB);
dispmaxdiff('[16KB, upcxx]', difference_16KB_upcxx, num_processes)