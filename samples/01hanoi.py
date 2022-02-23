def solve_hanoi_recur(towers, start, goal, ndisks):
    if ndisks == 1:
        return [(start, goal)]

    indices = list(range(0, len(towers)))
    indices.remove(start)
    indices.remove(goal)
    mid = max(indices)
    ops = []

    towers_start = towers[:]
    towers_start[start] = towers[start][:1 - ndisks]
    towers_start[mid] += towers[start][1 - ndisks:]
    ops += solve_hanoi_recur(towers_start, start, mid, ndisks - 1)

    towers_mid = towers_start[:]
    towers_mid[start] = towers_start[start][:-1]
    towers_mid[goal] += towers_start[start][-1:]
    ops += [(start, goal)]

    towers_goal = towers_mid[:]
    towers_goal[mid] = towers_mid[mid][:1 - ndisks]
    towers_goal[goal] += towers_mid[mid][1 - ndisks:]
    ops += solve_hanoi_recur(towers_goal, mid, goal, ndisks - 1)

    return ops


def solve_hanoi(ntowers, ndisks):
    towers = [[] for _i in range(ntowers)]
    disks = list(reversed(range(ndisks)))
    towers[0] = disks
    return solve_hanoi_recur(towers, 0, len(towers) - 1, ndisks)


print(solve_hanoi(3, 4))
