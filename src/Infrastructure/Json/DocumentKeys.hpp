#pragma once

#include <string_view>

namespace sloptour::infrastructure::keys {

constexpr std::string_view FORMAT = "format";
constexpr std::string_view ID = "id";
constexpr std::string_view TITLE = "title";
constexpr std::string_view SUMMARY = "summary";
constexpr std::string_view KIND = "kind";
constexpr std::string_view REPOSITORY = "repository";
constexpr std::string_view SCOPE = "scope";
constexpr std::string_view AGENT = "agent";
constexpr std::string_view CREATED_AT = "created_at";
constexpr std::string_view CHAPTERS = "chapters";
constexpr std::string_view STEPS = "steps";
constexpr std::string_view BODY = "body";
constexpr std::string_view LOCATIONS = "locations";
constexpr std::string_view PATH = "path";
constexpr std::string_view START_LINE = "start_line";
constexpr std::string_view END_LINE = "end_line";
constexpr std::string_view NOTE = "note";
constexpr std::string_view COMMANDS = "commands";
constexpr std::string_view CHECKPOINT = "checkpoint";
constexpr std::string_view QUESTION = "question";
constexpr std::string_view ANSWER = "answer";
constexpr std::string_view GOAL = "goal";
constexpr std::string_view FOCUS_PATHS = "focus_paths";
constexpr std::string_view CURRENT = "current";
constexpr std::string_view VISITED = "visited";
constexpr std::string_view REVEALED = "revealed";
constexpr std::string_view EXCHANGES = "exchanges";
constexpr std::string_view QUIZ = "quiz";
constexpr std::string_view QUESTIONS = "questions";
constexpr std::string_view OPTIONS = "options";
constexpr std::string_view EXPLANATION = "explanation";
constexpr std::string_view QUIZ_ANSWERS = "quiz_answers";
constexpr std::string_view MODEL = "model";
constexpr std::string_view DEPTH = "depth";
constexpr std::string_view DETAIL = "detail";
constexpr std::string_view LANGUAGE = "language";

}
