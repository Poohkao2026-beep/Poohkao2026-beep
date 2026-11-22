# Featured projects

## Daybreak Study Planner

[Repository](https://github.com/Poohkao2026-beep/Daybreak-study-planner) ·
[Planner](https://daybreak-study-planner.vercel.app/planner) ·
[Preview architecture and access guide](https://github.com/Poohkao2026-beep/Daybreak-study-planner/blob/main/packages/planner-launcher/README.md)

Daybreak helps students choose what to study next, protect focus time, and review
progress. Its design aims for a calm interface with useful defaults and explicit
control over optional services.

### Architecture

The public repository contains a preview launcher and the earlier Foundation
source. The hosted preview is a newer implementation; running the repository's
root source starts the Foundation edition. These are distinct development targets.

| Layer | Hosted preview design |
| --- | --- |
| Interface | Next.js App Router, React, and JavaScript; English and Thai workspaces for tasks, courses, focus, and progress. |
| Local storage | IndexedDB keeps guest planner records in the browser. JSON export and restore provide a recovery path. |
| Account sync | Optional Firebase Authentication and account-scoped storage, with pending changes queued for synchronization. Server-side token verification and Firestore rules enforce access boundaries. |
| Optional services | Server routes handle Gemini study support and WeatherAPI requests. AI requires account access and consent; connectivity and provider availability apply. |
| Focus audio | Web Audio synthesizes local soundscapes in the browser. |
| Delivery | Vercel hosts the web preview. A small Node.js launcher opens it; the launcher is not a native planner application. |

The Foundation edition keeps a static planner separate from a Next.js Study
Support page. Its browser data and AI chat have separate boundaries. Consult the
[Foundation guide](https://github.com/Poohkao2026-beep/Daybreak-study-planner/blob/main/FOUNDATION.md)
when exploring that source rather than assuming it matches the hosted preview.

### Design philosophy

- **Local-first planning:** keep core records close to the user and make backup
  and restore accessible. First access and cloud features still need connectivity.
- **Optional AI:** provide study help through explicit consent. In the hosted
  preview, users may choose study context; AI is not required for ordinary planning.
- **Calm interaction:** make the next useful action clear, with adjustable focus
  sessions, predictable controls, and bilingual presentation.
- **Resilient boundaries:** isolate account synchronization and provider failures
  from everyday planning, and review imported changes before applying them.
- **Evidence before promises:** distinguish preview capabilities from verified
  production behavior, especially for synchronization and paid services.

### Useful areas to explore

Offline reopening and recovery, interrupted synchronization, accessible Thai and
English layouts, calendar import review, and graceful handling of provider outages
are valuable directions for continued work. See the preview guide for scope and
access requirements before proposing a change.
