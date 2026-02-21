# DEPLOY TEST VERSION

from flask import Flask, request, jsonify, send_from_directory
import os

from flask_cors import CORS

app = Flask(__name__)
CORS(app)

# -------------------------------------------------
# HOME ROUTE (CHECK BACKEND STATUS)
# -------------------------------------------------
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
FRONTEND_DIR = os.path.join(BASE_DIR, "../frontend/.dist")

@app.route("/")
def serve_frontend():
    return send_from_directory(FRONTEND_DIR, "index.html")

# -------------------------------------------------
# FIRST FIT ALGORITHM (MATCHES C CODE)
# -------------------------------------------------
def first_fit(blocks, processes):
    allocation = [-1] * len(processes)

    for i in range(len(processes)):
        for j in range(len(blocks)):
            if blocks[j] >= processes[i]:
                allocation[i] = j
                blocks[j] -= processes[i]
                break

    return allocation, blocks


# -------------------------------------------------
# BEST FIT ALGORITHM (MATCHES C CODE)
# -------------------------------------------------
def best_fit(blocks, processes):
    allocation = [-1] * len(processes)

    for i in range(len(processes)):
        best_idx = -1
        for j in range(len(blocks)):
            if blocks[j] >= processes[i]:
                if best_idx == -1 or blocks[j] < blocks[best_idx]:
                    best_idx = j

        if best_idx != -1:
            allocation[i] = best_idx
            blocks[best_idx] -= processes[i]

    return allocation, blocks


# -------------------------------------------------
# WORST FIT ALGORITHM (MATCHES C CODE)
# -------------------------------------------------
def worst_fit(blocks, processes):
    allocation = [-1] * len(processes)

    for i in range(len(processes)):
        worst_idx = -1
        for j in range(len(blocks)):
            if blocks[j] >= processes[i]:
                if worst_idx == -1 or blocks[j] > blocks[worst_idx]:
                    worst_idx = j

        if worst_idx != -1:
            allocation[i] = worst_idx
            blocks[worst_idx] -= processes[i]

    return allocation, blocks


# -------------------------------------------------
# FORMAT RESULT (COMMON FOR ALL ALGORITHMS)
# -------------------------------------------------
def format_result(processes, allocation):
    result = []

    for i in range(len(processes)):
        if allocation[i] != -1:
            result.append(
                f"Process {i+1} ({processes[i]} KB) -> Block {allocation[i]+1}"
            )
        else:
            result.append(
                f"Process {i+1} ({processes[i]} KB) -> Not Allocated"
            )

    return result


# -------------------------------------------------
# FIRST FIT API
# -------------------------------------------------
@app.route("/firstfit", methods=["POST"])
def api_first_fit():
    data = request.json
    blocks = data["blocks"]
    processes = data["processes"]

    allocation, remaining = first_fit(blocks.copy(), processes)

    return jsonify({
        "algorithm": "First Fit",
        "allocation": format_result(processes, allocation),
        "remaining_blocks": remaining
    })


# -------------------------------------------------
# BEST FIT API
# -------------------------------------------------
@app.route("/bestfit", methods=["POST"])
def api_best_fit():
    data = request.json
    blocks = data["blocks"]
    processes = data["processes"]

    allocation, remaining = best_fit(blocks.copy(), processes)

    return jsonify({
        "algorithm": "Best Fit",
        "allocation": format_result(processes, allocation),
        "remaining_blocks": remaining
    })


# -------------------------------------------------
# WORST FIT API
# -------------------------------------------------
@app.route("/worstfit", methods=["POST"])
def api_worst_fit():
    data = request.json
    blocks = data["blocks"]
    processes = data["processes"]

    allocation, remaining = worst_fit(blocks.copy(), processes)

    return jsonify({
        "algorithm": "Worst Fit",
        "allocation": format_result(processes, allocation),
        "remaining_blocks": remaining
    })


# -------------------------------------------------
# COMPARE ALL API (UTILIZATION BASED COMPARISON)
# -------------------------------------------------
@app.route("/compare", methods=["POST"])
def api_compare_all():
    data = request.json
    blocks = data["blocks"]
    processes = data["processes"]

    total_memory = sum(blocks)

    # Run all algorithms
    ff_alloc, ff_remaining = first_fit(blocks.copy(), processes)
    bf_alloc, bf_remaining = best_fit(blocks.copy(), processes)
    wf_alloc, wf_remaining = worst_fit(blocks.copy(), processes)

    # Helper function to calculate stats
    def calculate_stats(allocation, remaining_blocks):
        allocated_memory = 0
        allocated_processes = 0

        for i in range(len(allocation)):
            if allocation[i] != -1:
                allocated_memory += processes[i]
                allocated_processes += 1

        wasted_memory = sum(remaining_blocks)
        utilization = round((allocated_memory / total_memory) * 100, 2) if total_memory > 0 else 0

        return {
            "allocated_processes": allocated_processes,
            "allocated_memory": allocated_memory,
            "wasted_memory": wasted_memory,
            "utilization_percent": utilization
        }

    ff_stats = calculate_stats(ff_alloc, ff_remaining)
    bf_stats = calculate_stats(bf_alloc, bf_remaining)
    wf_stats = calculate_stats(wf_alloc, wf_remaining)

    # Determine best algorithm (highest utilization)
    best_algo = max(
        [("First Fit", ff_stats["utilization_percent"]),
         ("Best Fit", bf_stats["utilization_percent"]),
         ("Worst Fit", wf_stats["utilization_percent"])],
        key=lambda x: x[1]
    )[0]

    return jsonify({
        "comparison_type": "Memory Utilization Based",
        "total_memory": total_memory,
        "First Fit": ff_stats,
        "Best Fit": bf_stats,
        "Worst Fit": wf_stats,
        "best_algorithm": best_algo
    })


# -------------------------------------------------
# RUN SERVER
# -------------------------------------------------
if __name__ == "__main__":
    port = int(os.environ.get("PORT", 10000))
    app.run(host="0.0.0.0", port=port)