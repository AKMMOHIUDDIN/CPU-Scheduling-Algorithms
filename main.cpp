#include <bits/stdc++.h>
using namespace std;

struct Process {
    int pid;
    int arrival;
    int burst;
    int remaining;
    int priority;
    int waiting = 0;
    int turnaround = 0;
    int completion = 0;
};

vector<int> gantt;

void printGantt() {
    cout << "\nGantt Chart:\n|";
    for (int p : gantt)
        cout << " P" << p << " |";
    cout << "\n";
}

void printTable(vector<Process>& p) {
    cout << "\nPID\tAT\tBT\tWT\tTAT\n";
    for (auto &x : p) {
        cout << x.pid << "\t" << x.arrival << "\t"
             << x.burst << "\t" << x.waiting << "\t"
             << x.turnaround << "\n";
    }
}

/* ================= FCFS ================= */
void FCFS(vector<Process> p) {
    sort(p.begin(), p.end(), [](auto &a, auto &b){
        return a.arrival < b.arrival;
    });

    int time = 0;
    for (auto &x : p) {
        time = max(time, x.arrival);
        x.waiting = time - x.arrival;
        time += x.burst;
        x.turnaround = x.waiting + x.burst;
        gantt.push_back(x.pid);
    }
    printTable(p);
    printGantt();
}

/* ================= Round Robin ================= */
void RoundRobin(vector<Process> p, int tq) {
    queue<int> q;
    int time = 0, n = p.size();
    vector<bool> inQ(n, false);

    for (int i = 0; i < n; i++)
        p[i].remaining = p[i].burst;

    q.push(0);
    inQ[0] = true;

    while (!q.empty()) {
        int i = q.front(); q.pop();
        gantt.push_back(p[i].pid);

        int exec = min(tq, p[i].remaining);
        p[i].remaining -= exec;
        time += exec;

        for (int j = 0; j < n; j++) {
            if (!inQ[j] && p[j].arrival <= time && p[j].remaining > 0) {
                q.push(j);
                inQ[j] = true;
            }
        }

        if (p[i].remaining > 0)
            q.push(i);
        else {
            p[i].completion = time;
            p[i].turnaround = time - p[i].arrival;
            p[i].waiting = p[i].turnaround - p[i].burst;
        }
    }
    printTable(p);
    printGantt();
}

/* ================= SPN ================= */
void SPN(vector<Process> p) {
    int time = 0, completed = 0;
    int n = p.size();
    vector<bool> done(n, false);

    while (completed < n) {
        int idx = -1, mn = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (!done[i] && p[i].arrival <= time && p[i].burst < mn) {
                mn = p[i].burst;
                idx = i;
            }
        }
        time += p[idx].burst;
        p[idx].completion = time;
        p[idx].turnaround = time - p[idx].arrival;
        p[idx].waiting = p[idx].turnaround - p[idx].burst;
        done[idx] = true;
        completed++;
        gantt.push_back(p[idx].pid);
    }
    printTable(p);
    printGantt();
}

/* ================= SRT ================= */
void SRT(vector<Process> p) {
    int time = 0, completed = 0;
    int n = p.size();
    for (auto &x : p) x.remaining = x.burst;

    while (completed < n) {
        int idx = -1, mn = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival <= time && p[i].remaining > 0 && p[i].remaining < mn) {
                mn = p[i].remaining;
                idx = i;
            }
        }
        gantt.push_back(p[idx].pid);
        p[idx].remaining--;
        time++;

        if (p[idx].remaining == 0) {
            completed++;
            p[idx].completion = time;
            p[idx].turnaround = time - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
        }
    }
    printTable(p);
    printGantt();
}

/* ================= HRRN ================= */
void HRRN(vector<Process> p) {
    int time = 0, completed = 0;
    int n = p.size();
    vector<bool> done(n, false);

    while (completed < n) {
        double mx = -1;
        int idx = -1;
        for (int i = 0; i < n; i++) {
            if (!done[i] && p[i].arrival <= time) {
                double r = (double)(time - p[i].arrival + p[i].burst) / p[i].burst;
                if (r > mx) {
                    mx = r;
                    idx = i;
                }
            }
        }
        time += p[idx].burst;
        p[idx].completion = time;
        p[idx].turnaround = time - p[idx].arrival;
        p[idx].waiting = p[idx].turnaround - p[idx].burst;
        done[idx] = true;
        completed++;
        gantt.push_back(p[idx].pid);
    }
    printTable(p);
    printGantt();
}

/* ================= Feedback (MLFQ) ================= */
void Feedback(vector<Process> p) {
    queue<int> q1, q2, q3;
    int time = 0;
    int n = p.size();
    for (int i = 0; i < n; i++) {
        p[i].remaining = p[i].burst;
        q1.push(i);
    }

    while (!q1.empty() || !q2.empty() || !q3.empty()) {
        int i;
        if (!q1.empty()) { i = q1.front(); q1.pop(); }
        else if (!q2.empty()) { i = q2.front(); q2.pop(); }
        else { i = q3.front(); q3.pop(); }

        gantt.push_back(p[i].pid);
        p[i].remaining--;
        time++;

        if (p[i].remaining > 0) {
            if (q1.size() <= q2.size()) q2.push(i);
            else q3.push(i);
        } else {
            p[i].completion = time;
            p[i].turnaround = time - p[i].arrival;
            p[i].waiting = p[i].turnaround - p[i].burst;
        }
    }
    printTable(p);
    printGantt();
}

/* ================= Aging ================= */
void Aging(vector<Process> p) {
    int time = 0, completed = 0;
    int n = p.size();

    while (completed < n) {
        for (auto &x : p)
            if (x.arrival <= time && x.remaining > 0)
                x.priority++;

        int idx = -1, mx = -1;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival <= time && p[i].remaining > 0 && p[i].priority > mx) {
                mx = p[i].priority;
                idx = i;
            }
        }

        gantt.push_back(p[idx].pid);
        p[idx].remaining--;
        time++;

        if (p[idx].remaining == 0) {
            completed++;
            p[idx].completion = time;
            p[idx].turnaround = time - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
        }
    }
    printTable(p);
    printGantt();
}

/* ================= MAIN ================= */
int main() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> p(n);
    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        cout << "Arrival & Burst for P" << i + 1 << ": ";
        cin >> p[i].arrival >> p[i].burst;
        p[i].remaining = p[i].burst;
        p[i].priority = 0;
    }

    cout << "\n1.FCFS 2.RR 3.SPN 4.SRT 5.HRRN 6.Feedback 7.Aging\nChoose: ";
    int ch; cin >> ch;

    if (ch == 1) FCFS(p);
    else if (ch == 2) {
        int tq; cout << "Time Quantum: "; cin >> tq;
        RoundRobin(p, tq);
    }
    else if (ch == 3) SPN(p);
    else if (ch == 4) SRT(p);
    else if (ch == 5) HRRN(p);
    else if (ch == 6) Feedback(p);
    else if (ch == 7) Aging(p);

    return 0;
}
