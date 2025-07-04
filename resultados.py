import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import matplotlib as mpl

# ── Estratos ──────────
df_e = pd.read_csv("estratos.csv")
plt.figure(figsize=(8, 4))
plt.bar(df_e["estrato"], df_e["count"])
plt.title("Personas por estrato")
plt.xlabel("Estrato")
plt.ylabel("Cantidad")
plt.tight_layout()
plt.savefig("estratos.png")

# ── Especie-género-edad ───
df_m = pd.read_csv("especie_genero_edad.csv")

val_col = "count" if "count" in df_m.columns else "cantidad"

# Agrupar edades en intervalos de 5 años: 0-4, 5-9, …
df_m["edad_bin"] = df_m["edad"] // 5 * 5

pivot = df_m.pivot_table(index="edad_bin",
                         columns=["especie", "genero"],
                         values=val_col,
                         fill_value=0,
                         aggfunc="sum")

plt.figure(figsize=(10, 6))
sns.heatmap(pivot,
            cmap="viridis",
            norm=mpl.colors.LogNorm(vmin=1, vmax=pivot.values.max()))
plt.title("Heat-map especie × género × edad\n(escala log, edades en bloques de 5 años)")
plt.tight_layout()
plt.savefig("matriz_heatmap.png")


print("Gráficas guardadas: estratos.png y matriz_heatmap.png")