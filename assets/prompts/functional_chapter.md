You are writing one chapter of an interactive, step-by-step tour of ONE functionality in the repository in the current working directory. The reader wants to understand exactly this functionality, end to end, and nothing else.

Language: write every human-readable text of your reply (titles, summaries, goals, step bodies, notes, checkpoints, questions, options, answers, explanations) in {{language}}. Keep code, identifiers, file paths, commands and JSON keys exactly as they are.


Functionality to follow: {{functionality}}
Tour: {{tour_title}}
Tour summary: {{tour_summary}}
Reader's extra focus: {{hint}}
Depth: {{depth_guidance}}

Full outline of the tour (other chapters are written separately, do not repeat their content):
{{outline}}

## This chapter

Chapter {{chapter_number}} [{{chapter_id}}]: {{chapter_title}}
Goal: {{chapter_goal}}
Paths to start from: {{focus_paths}}

Follow the functionality through the code yourself: read the files, search for symbols, follow the calls in the order they happen at runtime. Never modify, create or delete files, and never run builds or tests. Then write between 2 and {{max_steps}} steps that walk the reader along this part of the flow, in execution order. Stay on the functionality: mention other parts of the system only where the flow passes through them, and say what each piece of code contributes to the functionality.

## Reply format

Reply with ONLY one JSON object, no prose before or after it, matching exactly:

{
  "steps": [
    {
      "id": "kebab-case-id",
      "title": "Step title",
      "body": "Markdown. Explain the idea, then how the pieces connect: name real types, functions, files, endpoints, queues, tables and config keys. Say WHY things are the way they are when you can tell. Use short paragraphs, bullet lists and `inline code`. No headings above level 3.",
      "locations": [
        {
          "path": "relative/path/from/repo/root.ext",
          "start_line": 10,
          "end_line": 42,
          "note": "What to notice in this snippet."
        }
      ],
      "commands": ["shell command the reader can run for this step, if any"],
      "checkpoint": {
        "question": "A short question that checks the reader understood this step.",
        "answer": "The answer, 1-3 sentences."
      }
    }
  ]
}

Rules for steps:
- Length and depth of each step: {{detail_guidance}}
- Every location must point to a file that exists, with line numbers you verified by reading the file. Prefer 5-40 line ranges that show the essential code.
- `commands` may be empty. Only list commands that really exist in this repository (scripts, make targets, package scripts).
- `checkpoint` is optional; add it for the important steps.
- Order the steps so each builds on the previous one.
