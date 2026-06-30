"""
图12：盈亏平衡点分析图
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
OUTPUT_FILE = os.path.join(OUTPUT_DIR, 'fig12_breakeven.png')

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
# 第5年成本参数（单位：元）
fixed_cost = 800222          # 年固定成本（贷款偿还+人员+设备+税金等）
unit_price = 199             # 产品单价
unit_variable_cost = 85      # 单位变动成本（原材料等）
design_capacity = 15000      # 第5年设计生产能力（件）

# 产量范围（0 ~ 20000件）
quantity = np.linspace(0, 20000, 500)

# 计算收入、成本、利润
revenue = unit_price * quantity
total_cost = fixed_cost + unit_variable_cost * quantity
profit = revenue - total_cost

# 盈亏平衡点计算
# BEP(Q) = 固定成本 / (单价 - 单位变动成本)
break_even_qty = fixed_cost / (unit_price - unit_variable_cost)

# ============ 绘图函数 ============
def setup_style(ax):
    for spine in ['top', 'right']:
        ax.spines[spine].set_visible(False)
    ax.spines['left'].set_color(C['light_gray'])
    ax.spines['bottom'].set_color(C['light_gray'])
    ax.yaxis.grid(True, alpha=0.12, linestyle='--', color=C['grid'])
    ax.tick_params(colors='#666666', labelsize=9)

# ============ 绘图 ============
fig, ax = plt.subplots(figsize=(7, 4.8), dpi=150)
setup_style(ax)

# 销售收入线
ax.plot(quantity, revenue / 10000,
        color=C['positive'], linewidth=2.5, label='销售收入', zorder=5)

# 总成本线
ax.plot(quantity, total_cost / 10000,
        color=C['negative'], linewidth=2.5, label='总成本', zorder=5)

# 利润线（虚线）
ax.plot(quantity, profit / 10000,
        color=C['primary'], linewidth=1.8, linestyle='--', label='利润', zorder=4)

# 盈亏平衡点标注
ax.axvline(x=break_even_qty, color=C['accent'], linewidth=1, linestyle=':', alpha=0.8, zorder=3)
ax.axhline(y=fixed_cost / 10000,
           xmin=0, xmax=break_even_qty / 20000,
           color=C['accent'], linewidth=1, linestyle=':', alpha=0.8, zorder=3)
ax.annotate(f'盈亏平衡点\n{break_even_qty:.0f}件',
            xy=(break_even_qty, fixed_cost / 10000),
            xytext=(break_even_qty + 1500, fixed_cost / 10000 - 8),
            fontsize=10, fontweight='bold', color=C['accent'],
            arrowprops=dict(arrowstyle='->', color=C['accent'], lw=1.2))

# 第5年计划产量标注
ax.axvline(x=design_capacity, color=C['neutral'], linewidth=0.8, linestyle='-.', alpha=0.6, zorder=3)
ax.annotate(f'第5年计划产量\n{design_capacity}件',
            xy=(design_capacity, (unit_price * design_capacity) / 10000),
            xytext=(design_capacity - 4000, (unit_price * design_capacity) / 10000 + 5),
            fontsize=9, color=C['neutral'],
            arrowprops=dict(arrowstyle='->', color=C['neutral'], lw=0.8))

# 零利润线
ax.axhline(y=0, color=C['text'], linewidth=0.5, linestyle='-', alpha=0.3, zorder=2)

# 坐标轴标签
ax.set_xlabel('产量（件）', fontsize=11, color=C['text'])
ax.set_ylabel('金额（万元）', fontsize=11, color=C['text'])
ax.set_title(f'盈亏平衡点分析：产量{break_even_qty:.0f}件即达盈亏平衡',
             fontsize=13, fontweight='bold', color=C['text'], pad=15)

# 图例
ax.legend(loc='upper left', fontsize=9, framealpha=0.9, edgecolor='#ddd')
ax.set_xlim(0, 20000)

# 白色背景
fig.patch.set_facecolor('white')
ax.patch.set_facecolor('white')

fig.savefig(OUTPUT_FILE, dpi=150, facecolor='white', pad_inches=0.3)
plt.close(fig)

print(f"[OK] 图12已保存: {OUTPUT_FILE}")
print(f"     尺寸: {os.path.getsize(OUTPUT_FILE)/1024:.1f} KB")
print(f"     盈亏平衡产量: {break_even_qty:.0f}件")
print(f"     盈亏平衡利用率: {break_even_qty/design_capacity*100:.2f}%")
