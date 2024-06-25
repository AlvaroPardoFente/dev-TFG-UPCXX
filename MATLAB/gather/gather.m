%% Add common functions to the path

clearvars;
addpath("../include/")

%% ----------------------PRINTING----------------------

do_print = true;

%% Import and process data

int_size = 4;

figure_position = [680 458 480 240];

base_path = "../../results/gather/";

data = struct();
clean_data = struct();

data.mpi_1N_2n = importtable(base_path + "mpi_gather_1N_2n.csv");
data.upcxx_rget_1N_2n = importtable(base_path + "upcxx_gather_rget_1N_2n.csv");
data.upcxx_rget_binomial_1N_2n = importtable(base_path + "upcxx_gather_rget_binomial_1N_2n.csv");
data.upcxx_rget_no_copy_1N_2n = importtable(base_path + "upcxx_gather_rget_no_copy_1N_2n.csv");
data.upcxx_rput_1N_2n = importtable(base_path + "upcxx_gather_rput_1N_2n.csv");

data.mpi_2N_4n = importtable(base_path + "mpi_gather_2N_4n.csv");
data.upcxx_rget_2N_4n = importtable(base_path + "upcxx_gather_rget_2N_4n.csv");
data.upcxx_rget_binomial_2N_4n = importtable(base_path + "upcxx_gather_rget_binomial_2N_4n.csv");
data.upcxx_rget_no_copy_2N_4n = importtable(base_path + "upcxx_gather_rget_no_copy_2N_4n.csv");
data.upcxx_rput_2N_4n = importtable(base_path + "upcxx_gather_rput_2N_4n.csv");

data.mpi_4N_8n = importtable(base_path + "mpi_gather_4N_8n.csv");
data.upcxx_rget_4N_8n = importtable(base_path + "upcxx_gather_rget_4N_8n.csv");
data.upcxx_rget_binomial_4N_8n = importtable(base_path + "upcxx_gather_rget_binomial_4N_8n.csv");
data.upcxx_rget_no_copy_4N_8n = importtable(base_path + "upcxx_gather_rget_no_copy_4N_8n.csv");
data.upcxx_rput_4N_8n = importtable(base_path + "upcxx_gather_rput_4N_8n.csv");

data.mpi_6N_12n = importtable(base_path + "mpi_gather_6N_12n.csv");
data.upcxx_rget_6N_12n = importtable(base_path + "upcxx_gather_rget_6N_12n.csv");
data.upcxx_rget_binomial_6N_12n = importtable(base_path + "upcxx_gather_rget_binomial_6N_12n.csv");
data.upcxx_rget_no_copy_6N_12n = importtable(base_path + "upcxx_gather_rget_no_copy_6N_12n.csv");
data.upcxx_rput_6N_12n = importtable(base_path + "upcxx_gather_rput_6N_12n.csv");

data.mpi_8N_16n = importtable(base_path + "mpi_gather_8N_16n.csv");
data.upcxx_rget_8N_16n = importtable(base_path + "upcxx_gather_rget_8N_16n.csv");
data.upcxx_rget_binomial_8N_16n = importtable(base_path + "upcxx_gather_rget_binomial_8N_16n.csv");
data.upcxx_rget_no_copy_8N_16n = importtable(base_path + "upcxx_gather_rget_no_copy_8N_16n.csv");
data.upcxx_rput_8N_16n = importtable(base_path + "upcxx_gather_rput_8N_16n.csv");

data.mpi_10N_20n = importtable(base_path + "mpi_gather_10N_20n.csv");
data.upcxx_rget_10N_20n = importtable(base_path + "upcxx_gather_rget_10N_20n.csv");
data.upcxx_rget_binomial_10N_20n = importtable(base_path + "upcxx_gather_rget_binomial_10N_20n.csv");
data.upcxx_rget_no_copy_10N_20n = importtable(base_path + "upcxx_gather_rget_no_copy_10N_20n.csv");
data.upcxx_rput_10N_20n = importtable(base_path + "upcxx_gather_rput_10N_20n.csv");

data.mpi_12N_24n = importtable(base_path + "mpi_gather_12N_24n.csv");
data.upcxx_rget_12N_24n = importtable(base_path + "upcxx_gather_rget_12N_24n.csv");
data.upcxx_rget_binomial_12N_24n = importtable(base_path + "upcxx_gather_rget_binomial_12N_24n.csv");
data.upcxx_rget_no_copy_12N_24n = importtable(base_path + "upcxx_gather_rget_no_copy_12N_24n.csv");
data.upcxx_rput_12N_24n = importtable(base_path + "upcxx_gather_rput_12N_24n.csv");


unique_sizes = unique(data.mpi_1N_2n.Size);
unique_sizes_bytes = unique_sizes .* 4;

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
size_tick_labels = {"4", "16", "64", "256", "1K", "4K", "16K", "64K", "256K", "512K"};
legend_names = {"rget", "rget\_binomial", "rget\_no\_copy", "rput"};

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
lgd = legend(legend_names, "Location","southeast");
%lgd.FontSize = 7;
xlabel('Size(Bytes)');
ylabel('Bandwidth(B/s)');
if (~do_print)
    title('Mean bandwidth in N = 2 n = 4');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("gather_2N_4n", "-dpng");
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
lgd = legend(legend_names, "Location","southeast");
%lgd.FontSize = 7;
xlabel('Size(Bytes)');
ylabel('Bandwidth(B/s)');
if (~do_print)
    title('Mean bandwidth in N = 4 n = 8');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("gather_4N_8n", "-dpng");
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

%% Plot best upcxx against mpi

mpi_plot = figure("Position", figure_position);
plot_objects = gobjects(numel(fields), 1);

hold on

marker_i = 1;

marker = markers(marker_i);
marker_i = marker_i + 1;
mpi_8N_plot = plot(unique_sizes_bytes, bandwidth_mean.mpi_4N_8n, "--", "Marker", marker, 'DisplayName', formatted_fields{6});

marker = markers(marker_i);
marker_i = marker_i + 1;
upcxx_best_8N_plot = plot(unique_sizes_bytes, bandwidth_mean.upcxx_rput_4N_8n, "--", "Marker", marker, 'DisplayName', formatted_fields{10});

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
xlabel('Size(Bytes)');
ylabel('Bandwidth(B/s)');
if (~do_print)
    title('Mean bandwidth in best upcxx and mpi');
end
grid on;

% Create a data cursor object and update its properties
dcm = datacursormode(gcf);
set(dcm, 'UpdateFcn', @updatedcm)

if (do_print)
    print("gather_best", "-dpng");
end

%% 64B: all processes

% Extract bandwidth for 64 bytes
size_64_idx = unique_sizes_bytes == 64;

% Initialize arrays for MPI and UPCXX
mpi_bandwidth_64B = zeros(length(num_processes), 1);
upcxx_bandwidth_64B = zeros(length(num_processes), 1);

% Extract data for each process count
for i = 1:length(num_processes)
    mpi_field = ['mpi_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    upcxx_field = ['upcxx_rput_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    
    mpi_bandwidth_64B(i) = bandwidth_mean.(mpi_field)(size_64_idx);
    upcxx_bandwidth_64B(i) = bandwidth_mean.(upcxx_field)(size_64_idx);
end

% Plot bandwidth for 64 bytes against number of processes
figure('Position', figure_position);

plot(num_processes, mpi_bandwidth_64B, '--o', 'DisplayName', 'mpi')
hold on
plot(num_processes, upcxx_bandwidth_64B, '--x', 'DisplayName', 'upcxx')
set(gca, 'YScale', 'log')

remove_m_ticks();
xlim([min(num_processes) max(num_processes)]);
legend('Location', 'southwest');
xlabel('Number of Processes');
ylabel('Bandwidth(B/s)');
if (~do_print)
    title('Bandwidth for 64 bytes against number of processes');
end
grid on;

if (do_print)
    print("gather_64B_all", "-dpng");
end

%% Extract bandwidth for 16 KB
size_16KB_idx = unique_sizes_bytes == 16 * 1024;

% Initialize arrays for MPI and UPCXX
mpi_bandwidth_16KB = zeros(length(num_processes), 1);
upcxx_bandwidth_16KB = zeros(length(num_processes), 1);

% Extract data for each process count
for i = 1:length(num_processes)
    mpi_field = ['mpi_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    upcxx_field = ['upcxx_rput_' num2str(num_processes(i)/2) 'N_' num2str(num_processes(i)) 'n'];
    
    mpi_bandwidth_16KB(i) = bandwidth_mean.(mpi_field)(size_16KB_idx);
    upcxx_bandwidth_16KB(i) = bandwidth_mean.(upcxx_field)(size_16KB_idx);
end

%% Plot bandwidth for 16 KB against number of processes
figure('Position', figure_position);

plot(num_processes, mpi_bandwidth_16KB, '--o', 'DisplayName', 'mpi')
hold on
plot(num_processes, upcxx_bandwidth_16KB, '--x', 'DisplayName', 'upcxx')
set(gca, 'YScale', 'log')
remove_m_ticks();
xlim([min(num_processes) max(num_processes)])
legend('Location', 'southwest');
xlabel('Number of Processes');
ylabel('Bandwidth(B/s)');
if (~do_print)
    title('Bandwidth for 16 KB against number of processes');
end
grid on;

if (do_print)
    print("gather_16KB_all", "-dpng");
end