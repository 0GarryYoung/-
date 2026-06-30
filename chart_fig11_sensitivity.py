"""
图11：敏感性分析图
基于STM32心率血压手表项目财务评价数据
数据来源：见 数据来源说明.md
"""

import matplotlib
matplotlib.use('Agg', force=True)
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np
import os

# ============ 配置 ============
OUTPUT_DIR = os.path.dirname(os.path.abspath(__file__))
OUTPUT_FILE = os.path.join(OUTPUT_DIR, 'fig11_sensitivity.png')

fm.fontManager.__init__()
plt.rcParams['font.sans-serif'] = [
    'WenQuanYi Zen Hei',   # Linux 常见中文字体
    'Microsoft YaHei',     # Windows 微软雅黑
    'SimHei',              # Windows 黑体
    'DengXian',            # Windows 等线
    'DejaVu Sans',         # 兜底无衬线字体
]
plt.rcParams['axes.unicode_minus'] = False

C = {
    'primary': '#2E5C8A',
    'secondary': '#7D8F69',
    'accent': '#A67C52',
    'negative': '#C75B5B',
    'positive': '#5B8C6B',
    'neutral': '#8B7D9C',
    'light_gray': '#cccccc',
    'grid': '#aaaaaa',
    'text': '#333333',
}

# ============ 数据 ============
# 敏感性分析数据
# 各因素在不同变化率下的内部收益率（IRR%）
change_rates = np.array([-10, -5, 0, 5, 10])  # 变化率 %

# 1. 销售价格变化（最敏感因素）
irr_price = np.array([12.85, 20.72, 28.34, 35.56, 42.56])

# 2. 原材料成本变化
irr_material = np.array([35.18, 31.82, 28.34, 24.72, 22.15])

# 3. 用人成本变化
irr_labor = np.array([39.12, 33.85, 28.34, 22.85, 18.52])

# 4. 其他成本变化
irr_other = np.array([31.28, 29.82, 28.34, 26.88, 25.62])

# 5. 设备成本变化（最不敏感因素）
irr_equip = np.array([28.47, 28.41, 28.34, 28.28, 28.21])

# 基准收益率
MARR = 12

# ============ 绘图函数 ============
def setup_style(ax):
    for spine in ['top', 'right']:
        ax.spines[spine].set_visible(False)
    ax.spines['left'].set_color(C['light_gray'])
    ax.spines['bottom'].set_color(C['light_gray'])
    ax.yaxis.grid(True, alpha=0.12, linestyle='--', color=C['grid'])
    ax.tick_params(colors='#666666', labelsize=9)

# ============ 绘图 ============
fig, ax = plt.subplots(figsize=(7, 4.5), dpi=150)
setup_style(ax)

# 绘制5条敏感性曲线
ax.plot(change_rates, irr_price,
        color=C['negative'], linewidth=2.5, marker='s', markersize=6,
        label='销售价格变化', zorder=5)
ax.plot(change_rates, irr_material,
        color=C['accent'], linewidth=2, marker='^', markersize=6,
        label='原材料成本变化', zorder=5)
ax.plot(change_rates, irr_labor,
        color=C['primary'], linewidth=2, marker='o', markersize=6,
        label='用人成本变化', zorder=5)
ax.plot(change_rates, irr_other,
        color=C['neutral'], linewidth=2, marker='D', markersize=6,
        label='其他成本变化', zorder=5)
ax.plot(change_rates, irr_equip,
        color=C['secondary'], linewidth=2, marker='v', markersize=6,
        label='设备成本变化', zorder=5)

# 基准收益率参考线
ax.axhline(y=MARR, color=C['text'], linewidth=0.8, linestyle='--', alpha=0.5, zorder=3)
ax.annotate(f'基准收益率 {MARR}%',
            xy=(8, MARR), xytext=(0, 8),
            textcoords='offset points', fontsize=9, color='#666')

# 坐标轴标签
ax.set_xlabel('不确定因素变化率（%）', fontsize=11, color=C['text'])
ax.set_ylabel('内部收益率 IRR（%）', fontsize=11, color=C['text'])
ax.set_title('敏感性分析：销售价格对IRR影响最显著',
             fontsize=13, fontweight='bold', color=C['text'], pad=15)
ax.set_xticks(change_rates)

# 图例
ax.legend(loc='lower left', fontsize=9, framealpha=0.9, edgecolor='#ddd')

# 白色背景
fig.patch.set_facecolor('white')
ax.patch.set_facecolor('white')

fig.savefig(OUTPUT_FILE, dpi=150, facecolor='white', pad_inches=0.3)
plt.close(fig)

print(f"[OK] 图11已保存: {OUTPUT_FILE}")
print(f"     尺寸: {os.path.getsize(OUTPUT_FILE)/1024:.1f} KB")
