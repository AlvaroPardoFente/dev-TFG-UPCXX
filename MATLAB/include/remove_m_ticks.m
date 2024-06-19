function remove_m_ticks()
ax = gca;
ax.XMinorTick = "off";
ax.YMinorTick = "off";
ax.XMinorGrid = "off";
ax.YMinorGrid = "off";
xtickangle(45);
end