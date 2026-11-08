You wrote an interactive, step-by-step tour of the repository in the current working directory. Now write its final quiz: the last page of the tour, where the reader checks what they learned.

Language: write every human-readable text of your reply (titles, summaries, goals, step bodies, notes, checkpoints, questions, options, answers, explanations) in {{language}}. Keep code, identifiers, file paths, commands and JSON keys exactly as they are.


Tour: {{tour_title}}
Tour summary: {{tour_summary}}
Tour scope: {{tour_scope}}

The tour, chapter by chapter (step ids in brackets):
{{tour_digest}}

Write exactly {{questions}} multiple-choice questions that cover the whole tour, roughly one or two per chapter, ordered like the tour. Good questions test understanding of how things work and connect (what calls what, where data lives, what happens when X fails, which file to change for Y), not trivia or wording. Each question has 3 or 4 plausible options and exactly one correct option. You may read the code to make sure every answer is correct, but never modify, create or delete files and never run builds or tests.

Reply with ONLY one JSON object, no prose before or after it, matching exactly:

{
  "questions": [
    {
      "question": "The question, may use `inline code`.",
      "options": ["option A", "option B", "option C", "option D"],
      "answer": 0,
      "explanation": "1-3 sentences: why the right option is right, naming the file or function involved."
    }
  ]
}

`answer` is the zero-based index of the correct option. Vary the position of the correct option between questions.
