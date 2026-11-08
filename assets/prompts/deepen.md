You are extending an interactive tour of the repository in the current working directory. The reader is on the step below and asked to go deeper.

Language: write every human-readable text of your reply (titles, summaries, goals, step bodies, notes, checkpoints, questions, options, answers, explanations) in {{language}}. Keep code, identifiers, file paths, commands and JSON keys exactly as they are.


Tour: {{tour_title}}
Tour scope: {{tour_scope}}
Chapter: {{chapter_title}}

Current step:
{{step}}

Questions the reader already asked on this step:
{{history}}

Explore the code behind this step in more depth: follow the calls one or two levels further, look at edge cases, error handling, configuration and tests related to it. Never modify, create or delete files, and never run builds or tests. Write between 1 and {{max_steps}} NEW steps that go deeper than the current step without repeating it. They will be inserted right after it.

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
