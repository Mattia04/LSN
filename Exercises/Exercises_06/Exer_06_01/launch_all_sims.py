import shutil
import subprocess
from pathlib import Path

# Simulation executable
SIM_EXEC = "./simulator.exe"

# Input/output paths
TEMPLATE_INPUT = Path("./input.dat")
INPUT_FILE = Path("./INPUT/input.dat")
OUTPUT_FILES = [
    "magnetization.dat",
    "specific_heat.dat",
    "susceptibility.dat",
    "total_energy.dat",
]
OUTPUT_FILES = [Path("./OUTPUT/" + dir_) for dir_ in OUTPUT_FILES]

# Directory where results will be stored
RESULTS_DIR = Path("RESULTS")


# =========================
# FUNCTIONS
# =========================
def modify_input(template_path, input_path, temperature, restart, h, metro):
    """
    Read template input file, modify parameters, write new input file.
    """
    with open(template_path, "r") as f:
        content = f.read()

    # Example replacements (adapt to your file format)
    content = (
        content.replace("__H__", str(h))
        .replace("__RESTART__", str(int(restart)))
        .replace("__TEMPERATURE__", f"{temperature:.2f}")
        .replace("__METRO__", str(metro))
    )

    with open(input_path, "w") as f:
        f.write(content)


def run_simulation():
    """
    Run the simulation and wait for completion.
    """
    try:
        result = subprocess.run(
            [SIM_EXEC],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=True,  # raises error if simulation fails
            cwd="./SOURCE/",
        )

        print(result.stdout)

    except subprocess.CalledProcessError as e:
        print("Simulation failed!")
        print(e.stderr)
        raise


def save_output(temperature, field, metro):
    """
    Copy and rename output file to results directory.
    """
    RESULTS_DIR.mkdir(exist_ok=True)

    for out_file in OUTPUT_FILES:
        new_name = out_file.name.replace(
            ".",
            f"_T{temperature:.2f}_H{field:.2f}_{'metro' if metro == 2 else 'gibbs'}.",
        )
        shutil.copy(out_file, RESULTS_DIR / new_name)


# =========================
# MAIN LOOP
# =========================
def main():
    # all temperatures to use
    temps = [i / 20 for i in range(10, 41)][::-1]

    for field in [0.0, 0.02]:  # magnetic field
        for metro in [2, 3]:  # choose simulation type metropolis (2) or gibbs (3)
            for i, temp in enumerate(temps):
                print(
                    f"\n=== Running configuration T={temp:.2f} H={field:.2f} {'metro' if metro == 2 else 'gibbs'} ==="
                )

                modify_input(
                    TEMPLATE_INPUT, INPUT_FILE, temp, 0 if i == 0 else 1, field, metro
                )

                run_simulation()

                save_output(temp, field, metro)


if __name__ == "__main__":
    main()
