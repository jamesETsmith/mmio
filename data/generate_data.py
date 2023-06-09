from scipy.sparse import random
from scipy.io import mmwrite


def main():
    dens = 0.3
    for n in range(15, 16):
        m = random(2**n, 2**n, dens)
        mmwrite(f"n_{n}_{dens}.mtx", m)


if __name__ == "__main__":
    main()
