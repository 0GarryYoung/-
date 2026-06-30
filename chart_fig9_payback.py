"""
图9：动态投资回收期曲线
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
OUTPUT_FILE = os.path.join(OUTPUT_DIR, 'fig9_payback.png')

# 强制刷新字体缓存以加载中文字体
fm.fontManager.__init__()
plt.rcParams['font.sans-serif'] = [
    'WenQuanYi Zen Hei',   # Linux 常见中文字体
    'Microsoft YaHei',     # Windows 微软雅黑
    'SimHei',              # Windows 黑体
    'DengXian',            # Windows 等线
    'DejaVu Sans',         # 兜底无衬线字体
]
plt.rcParams['axes.unicode_minus'] = False

# 配色方案
C = {
    'primary': '#2E5C8A',      # 主色-深蓝
    'secondary': '#7D8F69',    # 辅助-橄榄绿
    'accent': '#A67C52',       # 强调-暖棕
    'negative': '#C75B5B',     # 负值-暗红
    'positive': '#5B8C6B',     # 正值-暗绿
    'neutral': '#8B7D9C',      # 中性-紫灰
    'light_gray': '#cccccc',   # 浅灰
    'grid': '#aaaaaa',         # 网格
    'text': '#333333',         # 文字
}

# ============ 数据 ============
# 5年累计净现金流量现值（单位：元）
# 折现率 = 12%
years = np.array([0, 1, 2, 3, 4, 5])
cumulative_npv = np.array([
    0,           # 第0年（建设期）
    -139931.25,  # 第1年
    -75580.01,   # 第2年
    138509.47,   # 第3年
    486658.23,   # 第4年
    928991.69,   # 第5年
])

# 动态回收期 = 3.24年（通过二次拟合求得）
payback_period = 3.24

# ============ 绘图函数 ============
def setup_style(ax):
    """设置专业L型坐标轴风格"""
    for spine in ['top', 'right']:
        ax.spines[spine].set_visible(False)
    ax.spines['left'].set_color(C['light_gray'])
    ax.spines['bottom'].set_color(C['light_gray'])
    ax.yaxis.grid(True, alpha=0.12, linestyle='--', color=C['grid'])
    ax.tick_params(colors='#666666', labelsize=9)

# ============ 绘图 ============
fig, ax = plt.subplots(figsize=(7, 4.5), dpi=150)
setup_style(ax)

# 主曲线
ax.plot(years, cumulative_npv,
        color=C['primary'], linewidth=2.5, marker='o', markersize=7,
        markerfacecolor='white', markeredgecolor=C['primary'], markeredgewidth=2,
        zorder=5, label='累计净现金流量现值')

# 填充正负区域
ax.fill_between(years, 0, cumulative_npv,
                where=(cumulative_npv < 0), alpha=0.12, color=C['negative'])
ax.fill_between(years, 0, cumulative_npv,
                where=(cumulative_npv >= 0), alpha=0.12, color=C['positive'])

# 零线
ax.axhline(y=0, color=C['text'], linewidth=0.8, linestyle='-', alpha=0.5, zorder=3)

# 回收期标注线
ax.axvline(x=payback_period, color=C['accent'], linewidth=1, linestyle='--', alpha=0.7, zorder=3)

# 数据标签
labels = ['-139,931', '-75,580', '138,509', '486,658', '928,992']
offsets_y = [-65000, -60000, 55000, 55000, 55000]
offsets_x = [0, 0.12, 0, 0, -0.12]
for x, y, lab, oy, ox in zip(years[1:], cumulative_npv[1:], labels, offsets_y, offsets_x):
    ax.text(x + ox, y + oy, lab, ha='center', fontsize=8, color='#555')

# 回收期文字标注
ax.text(3.42, -140000, f'动态回收期 = {payback_period}年',
        fontsize=10, fontweight='bold', color=C['accent'])

# 坐标轴标签
ax.set_xlabel('年份', fontsize=11, color=C['text'])
ax.set_ylabel('累计净现金流量现值（元）', fontsize=11, color=C['text'])
ax.set_title('动态投资回收期曲线：3.24年实现收支平衡',
             fontsize=13, fontweight='bold', color=C['text'], pad=15)
ax.set_xticks(years)
ax.set_xlim(-0.3, 5.3)
ax.set_ylim(-220000, 1150000)

# 背景设置（白色背景，非透明）
fig.patch.set_facecolor('white')
ax.patch.set_facecolor('white')

# 保存
fig.savefig(OUTPUT_FILE, dpi=150, facecolor='white', pad_inches=0.3)
plt.close(fig)

print(f"[OK] 图9已保存: {OUTPUT_FILE}")
print(f"     尺寸: {os.path.getsize(OUTPUT_FILE)/1024:.1f} KB")
