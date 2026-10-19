import matplotlib.pyplot as plt
import numpy as np

data = np.genfromtxt("closed_loop.log", delimiter=",", names=True)
time = data["time"]

fig, axes = plt.subplots(3, 1, sharex=True, figsize=(10, 8))

axes[0].plot(time, data["target_q"], "--", label="Target q")
axes[0].plot(time, data["measured_q"], label="Measured q")
axes[0].plot(time, data["target_d"], "--", label="Target d")
axes[0].plot(time, data["measured_d"], label="Measured d")
axes[0].set_ylabel("Current [A]")

axes[1].plot(time, data["voltage_d"], label="Voltage d")
axes[1].plot(time, data["voltage_q"], label="Voltage q")
axes[1].set_ylabel("Voltage [V]")

axes[2].plot(time, data["integral_d"], label="Integral d")
axes[2].plot(time, data["integral_q"], label="Integral q")
axes[2].set_ylabel("Integral contribution [V]")
axes[2].set_xlabel("Time [s]")

for ax in axes:
    ax.grid(True)
    ax.legend()

fig.suptitle("Motor current control: simulated RL load")
fig.tight_layout()
fig.savefig("closed_loop.png", dpi=150)
plt.show()
