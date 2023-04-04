from scipy.sparse import random
from scipy.io import mmwrite


def main():
    dens = 0.3
    for n in range(2,5):

        m = random(10**n, 10**n, dens)
        mmwrite(f"d={dens}_n={n}.mtx", m)

if __name__ == "__main__":
    main()