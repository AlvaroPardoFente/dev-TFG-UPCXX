function ytick_range  = get_ytick_range(min, max)
min_power = floor(log10(min));
max_power = ceil(log10(max));
ytick_range = 10.^(min_power:max_power);
end