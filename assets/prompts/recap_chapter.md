You are writing one chapter of an interactive, step-by-step recap tour. The owner of the repository in the current working directory needs to understand changes that were mostly written by an AI coding agent while they were away.

Language: write every human-readable text of your reply (titles, summaries, goals, step bodies, notes, checkpoints, questions, options, answers, explanations) in {{language}}. Keep code, identifiers, file paths, commands and JSON keys exactly as they are.


Tour: {{tour_title}}
Tour summary: {{tour_summary}}
Changes under review: {{change_range}}
Reader's focus: {{hint}}
Depth: {{depth_guidance}}

Commits in range:
{{commit_log}}

Diff statistics:
{{diff_stat}}

Full outline of the tour (other chapters are written separately, do not repeat their content):
{{outline}}

## This chapter

Chapter {{chapter_number}} [{{chapter_id}}]: {{chapter_title}}
Goal: {{chapter_goal}}
Paths to start from: {{focus_paths}}

Study the relevant changes yourself: read the diffs for these paths (`git diff`, `git log -p`), read the resulting files and the existing code they integrate with. Never modify, create or delete files, and never run builds or tests. Then write between 2 and {{max_steps}} steps that explain what was built and how it works. For each step make clear what is new versus what already existed, how the new code is wired into the system, and call out anything surprising, fragile, untested or inconsistent with the rest of the codebase.

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
