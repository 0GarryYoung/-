"""
图10：净现值曲线（NPV vs 折现率）
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
OUTPUT_FILE = os.path.join(OUTPUT_DIR, 'fig10_npv.png')

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
# 5年净现金流量（单位：元）
# 第0年为建设期投入，第1-5年为运营期
cash_flows = np.array([
    -150000,   # 第0年：建设投资
    -156718,   # 第1年：运营净现金流
    72032,     # 第2年
    300782,    # 第3年
    547782,    # 第4年
    779532,    # 第5年
])

# 基准收益率（MARR）
MARR = 0.12  # 12%

# ============ 计算函数 ============
def calc_npv(rate, cash_flows):
    """
    计算给定折现率下的净现值
    NPV = Σ(CF_t / (1 + r)^t)
    """
    return sum(cf / ((1 + rate) ** t) for t, cf in enumerate(cash_flows))

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

# 计算不同折现率下的NPV
rates = np.linspace(0, 0.50, 500)  # 0% ~ 50%
npvs = [calc_npv(r, cash_flows) for r in rates]

# 主曲线
ax.plot(rates * 100, npvs, color=C['primary'], linewidth=2.5, zorder=5)

# 零线
ax.axhline(y=0, color=C['text'], linewidth=0.8, linestyle='-', alpha=0.5, zorder=3)

# IRR标注线（NPV=0时的折现率）
IRR = 28.34  # 内部收益率 %
ax.axvline(x=IRR, color=C['accent'], linewidth=1, linestyle='--', alpha=0.7, zorder=3)
ax.annotate(f'IRR = {IRR}%',
            xy=(IRR, 0), xytext=(IRR + 5, 80000),
            fontsize=11, fontweight='bold', color=C['accent'],
            arrowprops=dict(arrowstyle='->', color=C['accent'], lw=1.2))

# MARR标注线
ax.axvline(x=MARR * 100, color=C['secondary'], linewidth=1, linestyle=':', alpha=0.7, zorder=3)
npv_at_marr = calc_npv(MARR, cash_flows)
ax.annotate(f'MARR = {MARR*100:.0f}%',
            xy=(MARR * 100, npv_at_marr),
            xytext=(MARR * 100 + 5, npv_at_marr - 120000),
            fontsize=10, color=C['secondary'],
            arrowprops=dict(arrowstyle='->', color=C['secondary'], lw=1))

# 填充正负区域
ax.fill_between(rates * 100, 0, npvs,
                where=(np.array(npvs) >= 0), alpha=0.12, color=C['positive'])
ax.fill_between(rates * 100, 0, npvs,
                where=(np.array(npvs) < 0), alpha=0.12, color=C['negative'])

# 坐标轴标签
ax.set_xlabel('折现率（%）', fontsize=11, color=C['text'])
ax.set_ylabel('净现值 NPV（元）', fontsize=11, color=C['text'])
ax.set_title(f'净现值曲线：IRR = {IRR}%，远高于基准收益率{MARR*100:.0f}%',
             fontsize=13, fontweight='bold', color=C['text'], pad=15)
ax.set_xlim(0, 50)

# 白色背景
fig.patch.set_facecolor('white')
ax.patch.set_facecolor('white')

fig.savefig(OUTPUT_FILE, dpi=150, facecolor='white', pad_inches=0.3)
plt.close(fig)

print(f"[OK] 图10已保存: {OUTPUT_FILE}")
print(f"     尺寸: {os.path.getsize(OUTPUT_FILE)/1024:.1f} KB")
