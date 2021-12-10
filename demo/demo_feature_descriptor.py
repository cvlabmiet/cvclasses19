#!/usr/bin/python3

# DRAFT

import os
import sys
import json
import numpy as np
import matplotlib.pyplot as plt

def get_distances(values, cols):
  d = []

  dist = (lambda i, j : values[i] ^ values[j]) if type(values[0] is float) else (lambda x : bin(values[i] ^ values[j]).count("1"))
  descr = []
  for i in range(0, int(len(values) / cols)):
    descr.append([values[i * cols : (i + 1) * cols]])

  for i in range(0, len(descr)):
    for j in range(i+1, len(descr)):
      d.append(np.linalg.norm(np.array(descr[i]) - np.array(descr[j])))

  return d

def compare_descriptors(filename):
  data = dict()
  with open(filename) as f:
    data = json.load(f)

  hist_data = dict()
  for key, val in data.items():
    hist_data[key] = get_distances(val["data"], val["cols"])

  for key, val in hist_data.items():
    val = (val - np.min(val))/np.ptp(val)
    bins = np.linspace(min(val), max(val), 50)
    plt.hist(val, alpha=0.5, label=key)

  plt.legend()
  plt.show()

if __name__ == "__main__":
  if len(sys.argv) == 2:
    compare_descriptors(sys.argv[1])
