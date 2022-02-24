# 最初に作ったハノイの塔のソルバを再考したところ無意味な処理があったこ
# とが分かったので書き直した。


def solve_hanoi_recur(ndisks, start, mid, goal):
    if ndisks == 1:
        return [(start, goal)]

    ops = []
    ops += solve_hanoi_recur(ndisks - 1, start, goal, mid)
    ops += [(start, goal)]
    ops += solve_hanoi_recur(ndisks - 1, mid, start, goal)

    return ops


def solve_hanoi(ndisks):
    return solve_hanoi_recur(ndisks, 0, 1, 2)


print(solve_hanoi(4))
