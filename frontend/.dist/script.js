let chart;

/* ================================
   GET INPUT DATA
================================ */
function getInputData() {
    return {
        blocks: document.getElementById("blockSizes").value
            .split(",")
            .map(Number),
        processes: document.getElementById("processSizes").value
            .split(",")
            .map(Number)
    };
}

/* ================================
   DRAW MEMORY BLOCKS
================================ */
function drawMemory(blocks) {
    const container = document.getElementById("memoryContainer");
    container.innerHTML = "";

    blocks.forEach((size, i) => {
        const div = document.createElement("div");
        div.className = "memory-block";
        div.innerHTML = `Block ${i + 1}<br>${size} KB`;
        container.appendChild(div);
    });
}

/* ================================
   UPDATE PERFORMANCE STATS
================================ */
function updateStats(originalBlocks, remainingBlocks) {
    const totalOriginal = originalBlocks.reduce((a, b) => a + b, 0);
    const totalRemaining = remainingBlocks.reduce((a, b) => a + b, 0);
    const totalAllocated = totalOriginal - totalRemaining;
    const fragmentation =
        totalOriginal > 0
            ? ((totalRemaining / totalOriginal) * 100).toFixed(2)
            : 0;

    document.getElementById("allocatedMem").innerText =
        totalAllocated + " KB";
    document.getElementById("unusedMem").innerText =
        totalRemaining + " KB";
    document.getElementById("fragmentation").innerText =
        fragmentation + "%";
}

/* ================================
   DETAILED EXPLANATION
================================ */
function detailedExplanation(type) {
    let text = "";

    if (type === "first") {
        text = `FIRST FIT:
• Allocates first suitable memory block.
• Fast and simple approach.
• May lead to external fragmentation.
• Time Complexity: O(n × m).`;
    }

    if (type === "best") {
        text = `BEST FIT:
• Allocates smallest block that fits.
• Reduces leftover free memory.
• Requires scanning full memory.
• Time Complexity: O(n × m).`;
    }

    if (type === "worst") {
        text = `WORST FIT:
• Allocates largest available block.
• Leaves larger free fragments.
• Can waste large memory blocks.
• Time Complexity: O(n × m).`;
    }

    if (type === "compare") {
        text = `COMPARISON MODE:
Algorithms are compared based on MEMORY UTILIZATION PERCENTAGE.
Higher utilization = Better memory efficiency.`;
    }

    document.getElementById("explanationText").innerText = text;
}

/* ================================
   DRAW UTILIZATION CHART
================================ */
function drawChart(ffUtil, bfUtil, wfUtil) {
    const ctx = document.getElementById("comparisonChart");

    if (chart) {
        chart.destroy();
    }

    chart = new Chart(ctx, {
        type: "bar",
        data: {
            labels: ["First Fit", "Best Fit", "Worst Fit"],
            datasets: [
                {
                    label: "Memory Utilization (%)",
                    data: [ffUtil, bfUtil, wfUtil],
                    backgroundColor: [
                        "#3b82f6",
                        "#10b981",
                        "#ef4444",
                    ],
                },
            ],
        },
        options: {
            responsive: true,
            scales: {
                y: {
                    beginAtZero: true,
                    max: 100,
                    ticks: {
                        callback: function (value) {
                            return value + "%";
                        },
                    },
                },
            },
        },
    });
}

/* ================================
   RUN SINGLE ALGORITHM
================================ */
function runAlgo(endpoint, title, type) {
    const start = performance.now();
    const input = getInputData();

   fetch(`/${endpoint}`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(input),
    })
        .then((res) => res.json())
        .then((data) => {
            document.getElementById("output").value =
                title + "\n\n" + data.allocation.join("\n");

            drawMemory(data.remaining_blocks);
            updateStats(input.blocks, data.remaining_blocks);
            detailedExplanation(type);

            const end = performance.now();
            document.getElementById("execTime").innerText =
                (end - start).toFixed(2) + " ms";
        })
        .catch((err) => {
            console.error(err);
            alert("Error running algorithm.");
        });
}

function runFirstFit() {
    runAlgo("firstfit", "FIRST FIT RESULT", "first");
}

function runBestFit() {
    runAlgo("bestfit", "BEST FIT RESULT", "best");
}

function runWorstFit() {
    runAlgo("worstfit", "WORST FIT RESULT", "worst");
}

/* ================================
   RUN COMPARISON (UTILIZATION BASED)
================================ */
function runCompareAll() {
    fetch("/compare", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(getInputData()),
    })
        .then((res) => res.json())
        .then((data) => {
            let text = "MEMORY UTILIZATION COMPARISON\n";
            text += "====================================\n\n";

            text += "Total Memory: " + data.total_memory + " KB\n\n";

            ["First Fit", "Best Fit", "Worst Fit"].forEach((algo) => {
                let stats = data[algo];

                text += algo.toUpperCase() + "\n";
                text += "------------------------------------\n";
                text += 
                    "Allocated Processes: " +
                    stats.allocated_processes +
                    "\n";
                text +=
                    "Allocated Memory: " +
                    stats.allocated_memory +
                    " KB\n";
                text +=
                    "Wasted Memory: " +
                    stats.wasted_memory +
                    " KB\n";
                text +=
                    "Utilization: " +
                    stats.utilization_percent +
                    "%\n\n";
            });

            text += "🏆 BEST ALGORITHM: " + data.best_algorithm;

            document.getElementById("output").value = text;

            drawChart(
                data["First Fit"].utilization_percent,
                data["Best Fit"].utilization_percent,
                data["Worst Fit"].utilization_percent
            );

            detailedExplanation("compare");
        })
        .catch((err) => {
            console.error(err);
            alert("Comparison failed. Check backend.");
        });
}

/* ================================
   RESET FUNCTION
================================ */
function resetAll() {
    document.getElementById("output").value = "";
    document.getElementById("memoryContainer").innerHTML = "";
    document.getElementById("allocatedMem").innerText = "0 KB";
    document.getElementById("unusedMem").innerText = "0 KB";
    document.getElementById("fragmentation").innerText = "0%";
    document.getElementById("execTime").innerText = "0 ms";

    if (chart) {
        chart.destroy();
    }
}
