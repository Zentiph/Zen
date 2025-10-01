"""
A basic ticket adding/listing/closing system for the project.
If this project reaches a bigger scale, this will likely (hopefully)
be refactored/changed to a better method of doing things,
but for now this is the easiest way for me.
:authors: Gavin Borne
"""

import datetime
import yaml
import json
import sys
import os

from typing import TypedDict, Literal, TypeAlias, NotRequired

Urgency: TypeAlias = Literal["low", "medium", "high"]
Status: TypeAlias = Literal["open", "closed"]
SortMethod: TypeAlias = Literal["priority", "recent", "none"]


class Ticket(TypedDict):
    id: int
    title: str
    desc: str
    urgency: Urgency
    status: Status
    created: str
    closed: NotRequired[str]


PATH = "tickets.yaml"


def urgency_ord(urgency: Urgency) -> Literal[1, 2, 3]:
    if urgency == "low":
        return 1
    if urgency == "medium":
        return 2
    if urgency == "high":
        return 3

    sys.exit("Usage: ticket.py new <urgency:low|medium|high> <title> [desc...]")


def validate_urgency(urgency: str) -> Urgency:
    if urgency in {"l", "lo", "low"}:
        return "low"
    if urgency in {"m", "med", "medium"}:
        return "medium"
    if urgency in {"h", "hi", "high"}:
        return "high"

    sys.exit("Usage: ticket.py new <urgency:low|medium|high> <title> [desc...]")


def validate_sort(sort: str) -> SortMethod:
    if sort in {"p", "priority"}:
        return "priority"
    if sort in {"n", ""}:
        return "none"
    if sort in {"r", "recent"}:
        return "recent"

    sys.exit("Usage: ticket.py list [state:all|open] [sort:high|low]")


def load() -> list[Ticket]:
    if not os.path.exists(PATH):
        return []
    with open(PATH, "r", encoding="utf-8") as f:
        return yaml.safe_load(f) or []


def save(items: list[Ticket]):
    with open(PATH, "w", encoding="utf-8") as f:
        yaml.safe_dump(items, f, sort_keys=False)


def next_id(items: list[Ticket]):
    return max((t["id"] for t in items), default=0) + 1


def main():
    if len(sys.argv) < 2:
        sys.exit("Commands: new | list [open|all] | show <id> | close <id>")

    cmd = sys.argv[1]
    items = load()

    if cmd == "new":
        if len(sys.argv) < 4:
            sys.exit("Usage: ticket.py new <urgency:low|medium|high> <title> [desc...]")

        urg, title = validate_urgency(sys.argv[2]), sys.argv[3]
        desc = " ".join(sys.argv[4:]) if len(sys.argv) > 4 else ""
        tid = next_id(items)
        items.append(
            {
                "id": tid,
                "title": title,
                "desc": desc,
                "urgency": urg,
                "status": "open",
                "created": datetime.date.today().isoformat(),
            }
        )
        save(items)
        print(f"[ISSUE {tid}] {title} saved!")
        return

    if cmd == "list":
        state = sys.argv[2] if len(sys.argv) > 2 else "open"
        sort = validate_sort(sys.argv[3]) if len(sys.argv) > 3 else "none"

        # TODO: allow for multi sorts by parsing and sorting one at a time (varargs)
        # TODO allow for more filtering opts (like by urgency)
        if sort == "priority":
            iter_items = sorted(
                items, key=lambda t: urgency_ord(t["urgency"]), reverse=True
            )
        elif sort == "recent":
            iter_items = reversed(items)
        else:
            iter_items = items

        for t in iter_items:
            if state in ("all", t["status"]):
                print(
                    f"#{t['id']:>3}"
                    f"  [{t['urgency'][:1].upper()}]"
                    f"  {t['title']}"
                    f"  ({t['status']})"
                )
        return

    if cmd == "show":
        if len(sys.argv) != 3:
            sys.exit("Usage: ticket.py show <id>")

        tid = int(sys.argv[2])
        t = next((x for x in items if x["id"] == tid), None)
        if not t:
            sys.exit(f"ISSUE {tid} not found.")
        print(json.dumps(t, indent=2))
        return

    if cmd == "close":
        if len(sys.argv) < 3:
            sys.exit("Usage: ticket.py close <id>")
        tid = int(sys.argv[2])
        found = False
        for t in items:
            if t["id"] == tid:
                t["status"] = "closed"
                t["closed"] = datetime.date.today().isoformat()
                found = True
                break

        if not found:
            sys.exit(f"ISSUE {tid} not found.")
        save(items)
        print(f"Closed ISSUE {tid}!")
        return

    sys.exit("Commands: new | list [open|all] | show <id> | close <id>")


if __name__ == "__main__":
    main()
