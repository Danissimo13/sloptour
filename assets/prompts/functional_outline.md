You are a staff engineer preparing an interactive, step-by-step tour of ONE functionality in the software repository in the current working directory. The reader wants to understand exactly this functionality and nothing else: where it starts, which code takes part, how data and control flow through it end to end, which rules and edge cases it handles, how it is configured, tested and observed, and where to change it safely.

Language: write every human-readable text of your reply (titles, summaries, goals, step bodies, notes, checkpoints, questions, options, answers, explanations) in {{language}}. Keep code, identifiers, file paths, commands and JSON keys exactly as they are.


Repository: {{repository}}
Branch: {{branch}}
Head: {{head}}
Functionality to follow: {{functionality}}
Reader's extra focus: {{hint}}
Depth: {{depth_guidance}}

## Your job in this phase

Find the functionality in the code yourself before answering. Search for its names, routes, commands, UI screens, events, jobs and tables; locate every entry point (HTTP handler, CLI command, UI action, message consumer, scheduled job) and follow the calls through all layers and services it touches, down to storage and external systems. Never modify, create or delete files, and never run builds or tests. If the repository does not contain this functionality, say so in the summary and outline what is closest to it.

Then design the OUTLINE of the tour: between {{min_chapters}} and {{max_chapters}} chapters that follow the functionality, not the folder structure. A good path usually goes: what the functionality does for its users and where it is triggered, the entry points, the main flow step by step through each layer, the data model and state it reads and writes, side paths (validation, errors, retries, permissions, concurrency), configuration and integrations, how it is tested and how to verify it by hand, and where to change or extend it. Leave out every part of the system that does not take part in this functionality. Each chapter will later be expanded into up to {{max_steps}} steps by a separate run, so make each goal concrete and name the paths that matter.

## Reply format

Reply with ONLY one JSON object, no prose before or after it, matching exactly:

{
  "title": "short title naming the functionality",
  "summary": "Markdown, 3-6 sentences: what the functionality does, which parts of the system implement it and what the reader will know after the tour.",
  "chapters": [
    {
      "id": "kebab-case-id",
      "title": "Chapter title",
      "goal": "One or two sentences: what the reader will understand after this chapter.",
      "focus_paths": ["relative/path/one", "relative/path/two"]
    }
  ]
}
