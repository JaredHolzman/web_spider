import matplotlib.pyplot as plt

def make_plot():
    x = []
    y = []
    with open(file_name) as file:
        for line in file:
            words = line.split()
            x.append(words[0])
            y.append(words[1])

    plt.plot(x, y)
    plt.show()

make_plot()
