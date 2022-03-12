'''
Author: Vilewind
Date: 2022-03-12 15:10:35
LastEditors: Vilewind
LastEditTime: 2022-03-12 15:54:18
FilePath: /EvolutionaryVaccinationGame/src_py/network.py
Version:
Description: 使用python创建网络拓扑
'''

import random
import networkx as nx
import matplotlib.pyplot as plt


def random_subset(seq, m, rng):
    r'''
        从目标片段中获取指定数目个目标
    :Arguments:
                **seq** 目标片段
                **m** 抽取目标数
                **rng** random.Random() 实例
        :Returns:
                **targets** 抽取目标集合
    '''
    #< set为无序的不重复元素序列，可以避免重复#
    targets = set()
    while len(targets) < m:
        x = rng.choice(seq)
        targets.add(x)
    return targets


def born_ba(G, m, rnodes, n=None, p=1.0):
    r'''
        创建ba网络, 或者基于已有网络重建创建经历个体新生的ba网络
    :Arguments:
                **G** 初始网络, 为ba网络
                **m** 网络每个新加入节点的度
                **rnodes** 被移除的节点集合
                **n** 网络规模
                **p** 新生的概率
    :Returns:
                **G** 重建后的ba网络
    :Demo:
                OG = born_ba(None, 4, None, 10)
                                NG, rnodes = death_ba(OG, d)
                                G = born_ba(NG, m, rnodes,p = p)
    '''
    if G is None and n is not None:
        return nx.barabasi_albert_graph(n, m)
    #< 按照度分布来构建节点序列，如节点0度为2，节点1度为0，节点2度为1，输出结果为[0,0,2]#
    repeated_nodes = [n for n, d in G.degree() for _ in range(d)]
    #< random.Random() 实例#
    rng = random.Random()
    for node in rnodes:
        #<< 判断个体是否新生#
        if random.random() > p:
            continue
        targets = random_subset(repeated_nodes, m, rng)
        G.add_edges_from(zip([node]*4, targets))
        #<< 新加入节点后，会增加目标节点和新加入节点的度分布#
        repeated_nodes.extend(targets)
        repeated_nodes.extend([node] * m)
    return G


def death_ba(G, p):
    r'''
    经历个体死亡后的ba网络
    :Arguments:
                **G** ba 网络
                **p** 个体死亡概率
    :Returns:
                **G** 经历死亡后的网络
                **rnodes** 死亡的个体
    '''
    rnodes = []
    for node in G.nodes:
        if random.random() < p:
            rnodes.append(node)
    for node in rnodes:
        G.remove_node(node)
    return G, rnodes


##<TEST#
N = 1000
m = 10
G = born_ba(None, m, None, N)
plt.subplot(131)
ps = nx.spring_layout(G)
nx.draw(G, ps, with_labels=False, node_size=30)
DG, rnodes = death_ba(G, 0.05)
plt.subplot(132)
ps = nx.spring_layout(DG)
nx.draw(DG, ps, with_labels=False, node_size=30)
#< 防止死亡后出现大量的孤立节点，可以将孤立节点当作新节点加入#
for n, d in G.degree():
    if d == 0:
        print(n)
        # rnodes.append(n)
NG = born_ba(DG, m, rnodes)
plt.subplot(133)
ps = nx.spring_layout(NG)
nx.draw(NG, ps, with_labels=False, node_size=30)
for n, d in G.degree():
    if d == 0:
        print(n)
plt.show()
