%% Import and process data

figure_position = [680 458 480 240];

mpi_2n = importtable("mpi_broadcast_array_2n.csv");
upcxx_2n = importtable("upcxx_broadcast_array_2n.csv");

mpi_4n = importtable("mpi_broadcast_array_4n.csv");
upcxx_4n = importtable("upcxx_broadcast_array_4n.csv");

mpi_6n = importtable("mpi_broadcast_array_6n.csv");
upcxx_6n = importtable("upcxx_broadcast_array_6n.csv");

mpi_8n = importtable("mpi_broadcast_array_8n.csv");
upcxx_8n = importtable("upcxx_broadcast_array_8n.csv");

mpi_10n = importtable("mpi_broadcast_array_10n.csv");
upcxx_10n = importtable("upcxx_broadcast_array_10n.csv");

mpi_12n = importtable("mpi_broadcast_array_12n.csv");
upcxx_12n = importtable("upcxx_broadcast_array_12n.csv");

unique_sizes = unique(mpi_8n.Size);
unique_sizes_categorical = categorical(unique_sizes);

mpi_2n_clean_data = rmopersize(mpi_2n);
upcxx_2n_clean_data = rmopersize(upcxx_2n);

mpi_4n_clean_data = rmopersize(mpi_4n);
upcxx_4n_clean_data = rmopersize(upcxx_4n);

mpi_6n_clean_data = rmopersize(mpi_6n);
upcxx_6n_clean_data = rmopersize(upcxx_6n);

mpi_8n_clean_data = rmopersize(mpi_8n);
upcxx_8n_clean_data = rmopersize(upcxx_8n);

mpi_10n_clean_data = rmopersize(mpi_10n);
upcxx_10n_clean_data = rmopersize(upcxx_10n);

mpi_12n_clean_data = rmopersize(mpi_12n);
upcxx_12n_clean_data = rmopersize(upcxx_12n);

%% 8 nodes: Times in different sizes

mpi_bandwidth = mpi_8n_clean_data.Size ./ mpi_8n_clean_data.Time;
mpi_mean_bandwidth = arrayfun(@(size) mean(mpi_bandwidth(mpi_8n_clean_data.Size == size)), unique_sizes);
upcxx_bandwidth = upcxx_8n_clean_data.Size ./ upcxx_8n_clean_data.Time;
upcxx_mean_bandwidth = arrayfun(@(size) mean(upcxx_bandwidth(upcxx_8n_clean_data.Size == size)), unique_sizes);

mpi_mean_times = arrayfun(@(size) mean(mpi_8n_clean_data.Time(mpi_8n_clean_data.Size == size)), unique_sizes);
upcxx_mean_times = arrayfun(@(size) mean(upcxx_8n_clean_data.Time(upcxx_8n_clean_data.Size == size)), unique_sizes);

sizes_figure = figure("Position", figure_position);

%tiledlayout("flow");

%nexttile
%loglog(unique_sizes, mpi_mean_times, "-o", "DisplayName", "mpi mean")
%hold on
%loglog(unique_sizes, upcxx_mean_times, "-o", "DisplayName", "upcxx mean")

%legend("mpi", "upcxx");
%xlabel('Size');
%ylabel('Time (s)');
%title('Mean times without outliers');
%grid on;

%nexttile
plot(unique_sizes, mpi_mean_bandwidth, "--o")
hold on
plot(unique_sizes, upcxx_mean_bandwidth, "--o")
set(gca, "XScale", "log")
xlim([min(unique_sizes) max(unique_sizes)])
legend("mpi", "upcxx");
xlabel('Size');
ylabel('Bandwidth(4B/s)');
title('Mean bandwidth per size on 8 processes');
grid on;

% Calculate the directional relative difference for each size
directional_diff = (mpi_mean_bandwidth - upcxx_mean_bandwidth) ./ ((mpi_mean_bandwidth + upcxx_mean_bandwidth) / 2) * 100;

% Calculate the average percentage difference
average_directional_diff = mean(directional_diff);

% Display the average percentage difference
disp(['Average Percentage Difference in Bandwidth: ', num2str(average_directional_diff), '%']);

%nexttile
%bar(unique_sizes_categorical, [mpi_mean_bandwidth, upcxx_mean_bandwidth])
%legend("mpi", "upcxx");
%xlabel('Size');
%ylabel('Bandwidth(4B/s)');
%title('Mean bandwidth per size');
%grid on;

print("broadcast_sizes", "-dpng")

%% 4MB: Times in different number of nodes

num_nodes = [2, 4, 6, 8, 10, 12];
size = 4 * 1024 * 1024;

mpi_2n_4M = mpi_2n_clean_data(mpi_2n_clean_data.Size == size, :);
mpi_4n_4M = mpi_4n_clean_data(mpi_4n_clean_data.Size == size, :);
mpi_6n_4M = mpi_6n_clean_data(mpi_6n_clean_data.Size == size, :);
mpi_8n_4M = mpi_8n_clean_data(mpi_8n_clean_data.Size == size, :);
mpi_10n_4M = mpi_10n_clean_data(mpi_10n_clean_data.Size == size, :);
mpi_12n_4M = mpi_12n_clean_data(mpi_12n_clean_data.Size == size, :);

upcxx_2n_4M = upcxx_2n_clean_data(upcxx_2n_clean_data.Size == size, :);
upcxx_4n_4M = upcxx_4n_clean_data(upcxx_4n_clean_data.Size == size, :);
upcxx_6n_4M = upcxx_6n_clean_data(upcxx_6n_clean_data.Size == size, :);
upcxx_8n_4M = upcxx_8n_clean_data(upcxx_8n_clean_data.Size == size, :);
upcxx_10n_4M = upcxx_10n_clean_data(upcxx_10n_clean_data.Size == size, :);
upcxx_12n_4M = upcxx_12n_clean_data(upcxx_12n_clean_data.Size == size, :);

mpi_mean_times_4M = [
    mean(mpi_2n_4M.Time);
    mean(mpi_4n_4M.Time);
    mean(mpi_6n_4M.Time);
    mean(mpi_8n_4M.Time);
    mean(mpi_10n_4M.Time);
    mean(mpi_12n_4M.Time)
];

upcxx_mean_times_4M = [
    mean(upcxx_2n_4M.Time);
    mean(upcxx_4n_4M.Time);
    mean(upcxx_6n_4M.Time);
    mean(upcxx_8n_4M.Time);
    mean(upcxx_10n_4M.Time);
    mean(upcxx_12n_4M.Time)
];

% Calculate the directional relative difference for each node count
directional_diff = (mpi_mean_times_4M - upcxx_mean_times_4M) ./ ((mpi_mean_times_4M + upcxx_mean_times_4M) / 2) * 100;

% Calculate the average percentage difference
average_directional_diff = mean(directional_diff);

% Display the average percentage difference
disp(['Average Percentage Difference in Mean Times: ', num2str(average_directional_diff), '%']);

nodes_figure = figure("Position", figure_position);
bar(num_nodes, [mpi_mean_times_4M, upcxx_mean_times_4M]);
hold on;
xlabel('Number of Nodes');
ylabel('Mean Time (s)');
title('Mean Times for Size = 4M Across Different Node Counts');
legend("mpi", "upcxx", "Location","northwest");
grid on;

print("broadcast_nodes", "-dpng")