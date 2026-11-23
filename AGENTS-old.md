# AGENTS.md — DRM-SMRTVRB

These instructions apply to every AI coding agent working in this repository.
They are mandatory. Do not treat them as suggestions.

## 1. Identify the Agent and Follow the Correct Startup Order

At the beginning of every task, identify the active model/agent as accurately as the
runtime allows. Do not guess or claim to be Qwen based only on a vague model family
name.

There are two startup paths:

### Path A — Qwen

If the active coding model is Qwen (including Qwen 3.5):

1. Before opening or reading any project Markdown documentation, inspect Git metadata
   and the working tree only:
   - `git status --short`
   - `git branch --show-current`
   - `git log -1 --oneline`
2. Do not read README, AGENT_HANDOVER, or any other `.md` file yet.
3. If the working tree has uncommitted changes, STOP. Do not stash, reset, clean,
   discard, overwrite, or otherwise disturb them. Report the state and wait.
4. Create a new task-specific branch and switch to it before reading project
   Markdown files or modifying project files.
5. Verify the active branch with `git branch --show-current`.
6. Only after the branch is created and verified, continue to Section 3
   (Documentation Understanding).

### Path B — Any non-Qwen agent

If the active model/agent is not Qwen:

1. Before substantial work, locate and read the existing token/usage-limit instructions
   in the repository's Markdown documentation. Search all relevant `.md` files for
   terms such as `token`, `usage`, `quota`, `context`, `15%`, `handover`, and
   `limit`. Do not assume the instructions are in a particular file.
2. Confirm in your own startup report that you have found and understood the
   repository's token-check procedure, including:
   - how the current remaining token/usage allowance is checked;
   - what source or interface provides the value;
   - the required stopping threshold (15%);
   - what handover must be produced when the threshold is reached.
3. Actually perform the available token/usage check before starting development.
   Merely saying that you will check later does not satisfy this requirement.
4. If the check cannot be accessed, its meaning is unclear, or the remaining allowance
   cannot be reliably determined, STOP before development. Explain what is unavailable.
   Do not invent a token API, fabricate a percentage, or claim a check passed.
5. If remaining allowance is below 15%, do not start or continue implementation.
   Stop work and produce the handover described in Section 6.
6. If the check is valid and remaining allowance is at least 15%, create and switch to
   a task-specific Git branch using Section 2.
7. Verify the active branch. Then continue to Section 3.

The existing repository token-check procedure is authoritative. Do not replace,
weaken, bypass, or silently reinterpret it. If these instructions and another
repository document appear inconsistent, report the conflict and follow the safer
rule: do not begin or continue development until clarified.


### Mandatory Handover Reading Protocol

Every agent working on DRM-SMRTVRB MUST read
`AGENT_HANDOVER.md` before beginning development.

This requirement applies to all agents, including
Codex, Antigravity, Copilot, and Qwen.

#### Startup Order

Agents MUST follow the startup order already
defined in their respective startup paths.

Qwen's special startup order MUST remain intact:
1. Inspect the current Git state.
2. Create and verify its dedicated task branch.
3. Read the project Markdown documentation,
   including AGENT_HANDOVER.md.

Other agents must follow their existing startup
requirements before proceeding to documentation.

#### Required Handover Review

Read these sections first:

1. CURRENT HANDOVER
2. NEXT AGENT INSTRUCTIONS

The HANDOVER HISTORY section is reference material
only and must not override the current handover.

After reading the handover, the agent MUST:

1. Identify the current development phase.
2. Identify the latest completed milestone.
3. Identify outstanding tasks and known blockers.
4. Review the stated next steps.
5. Inspect the actual Git branch and working tree.
6. Verify important handover claims against the
   actual repository state.
7. Identify outdated or contradictory information.

#### Startup Report

Before implementation, provide a concise report
containing:

- Current development phase.
- Latest verified completed milestone.
- Outstanding tasks.
- Known blockers or inconsistencies.
- Recommended next action.

Do not begin implementation until the handover
has been reviewed and the repository state checked.

#### Handover Authority

`AGENT_HANDOVER.md` is the canonical handover file.

The CURRENT HANDOVER section is authoritative.

Historical handovers are retained for context only.

If the handover conflicts with actual repository
state, investigate and report the discrepancy.

Never assume a task is completed solely because
an older handover says it was completed.

Never create alternative handover files unless
explicitly instructed by the project owner.


## 2. Mandatory Task Branch Isolation

Every agent must do its work on a dedicated task branch. No agent may develop
directly on `main`.

### Branch naming

Choose a short, descriptive name based on the actual assigned task, using a suitable
prefix, for example:

- `docs/reconciliation`
- `fix/space-automation`
- `feature/stereo-width-control`
- `test/windows-reaper-validation`
- `build/release-packaging`

Do not use a generic name such as `work`, `changes`, or `agent`.

### Safe branch creation

Before creating a branch:

1. Inspect the current branch and working tree.
2. Preserve all existing user or agent changes.
3. If the working tree is not clean, STOP and report it. Never automatically stash,
   reset, clean, checkout over, or discard existing changes.
4. If the repository is not on `main`, do not force a switch to `main`. Report the
   current branch and wait for direction unless the task owner has explicitly
   instructed otherwise.
5. If on `main` with a clean working tree, fetch the latest remote information and
   fast-forward local `main` from `origin/main` only when that operation is safe and
   possible. If it cannot be done safely, stop and report why.
6. Create a new branch from the updated `main`, then switch to it.
7. Verify the branch name before editing any project file.

If the intended branch name already exists, do not overwrite, reset, or repurpose it.
Inspect it and choose a distinct task-specific name, or stop if its state is unclear.

### Git restrictions

- Never commit directly to `main`.
- Never merge into `main`.
- Never force-push.
- Never delete branches or rewrite Git history unless the repository owner explicitly
  authorizes that specific action.
- Do not overwrite another agent's branch or work.
- Push the task branch only when the environment is authenticated and pushing is
  appropriate for the task. Report clearly if it was not pushed.
- Leave the repository on the task branch at the end of the task.
- Human approval is required before merging.

## 3. Documentation Understanding — Mandatory

After completing the applicable startup path and verifying the task branch, read and
understand the repository's Markdown documentation before implementation.

1. Discover all Markdown files in the repository, including root-level and nested
   `.md` files. Do not rely only on a hard-coded list.
2. Read all project-relevant Markdown files in full where practical. At minimum,
   inspect the complete contents of every project specification, status, roadmap,
   build, testing, issue, handover, and agent-instruction document.
3. Do not assume that reading `README.md` alone is sufficient.
4. Do not skip a document because it is long, appears repetitive, or seems unrelated
   based only on its filename.
5. If a file is exceptionally large, read it in manageable sections until the entire
   relevant content has been covered. State any file that could not be read and why.
6. Identify the purpose of each document and note contradictions, superseded
   instructions, unresolved decisions, and stale status claims.
7. Treat documentation as project guidance, not proof that a feature exists or works.

After reading the documentation, inspect source code, tests, build configuration,
and relevant CI workflows needed for the assigned task. Source code establishes
what is implemented; actual test/build output establishes what has been verified.

## 4. Confirm Understanding Before Implementation

Before changing implementation files, provide a concise internal/task-start summary
that demonstrates understanding of:

- the product's purpose and intended workflow;
- the relevant architecture and component boundaries;
- relevant DSP/audio behavior and parameter relationships, when applicable;
- the UI specification and interaction conventions, when applicable;
- build tools, dependencies, and supported targets;
- current implementation and verification status;
- known issues, constraints, and relevant roadmap items;
- the exact task scope and files likely to be affected.

Do not invent missing requirements. Ask for clarification when an unresolved decision
materially affects implementation.

Do not begin unrelated refactoring or feature work.

## 5. Documentation Reconciliation

Documentation reconciliation is a required part of project onboarding and must be
performed whenever an agent discovers conflicting, stale, incomplete, or inaccurate
project documentation. It is also mandatory when the assigned task explicitly asks
for documentation review or reconciliation.

Before changing documentation:

1. Read the relevant Markdown files and inspect the source code, build configuration,
   tests, and CI workflows needed to establish the actual project state.
2. Compare documentation claims against implementation and observed build/test results.
3. Identify contradictions, duplicated guidance, stale instructions, missing details,
   and claims that cannot currently be verified.
4. Treat source code as evidence of implementation and actual command output as
   evidence of validation. Do not convert an unverified claim into a verified fact.

When reconciling documentation:

- Update the relevant Markdown files so they agree on current project status, build
  instructions, architecture, parameters, testing, known issues, and next steps.
- Preserve historical information in the changelog and other historical records.
- Do not delete useful context merely to remove apparent contradictions; clarify what
  is current, what is historical, and what remains unverified.
- Do not change source code or product behavior as part of a documentation-only task.
- Keep `PROJECT_STATUS.md` aligned with evidence-backed implementation and validation
  status.
- Keep `AGENT_HANDOVER.md` actionable and specific about completed work, outstanding
  work, blockers, and the next recommended steps.
- Update `README.md` only when its user-facing project description or instructions
  are inaccurate or incomplete.
- Ensure internal file references, commands, paths, and documented parameter names
  match the repository.

Create or update a reconciliation report (for example,
`DOCUMENTATION_RECONCILIATION.md`) that records:

- Markdown files inspected and files modified;
- contradictions or stale statements found;
- corrections made and the evidence supporting them;
- unresolved discrepancies or decisions requiring human input;
- checks performed and their actual results.

Do not claim that all documentation is reconciled if any relevant file was not
inspected or any material discrepancy remains unresolved. Clearly list exceptions.

## 6. Development and Validation Rules

- Stay within the assigned task.
- Follow existing architecture, naming, formatting, and testing conventions.
- Do not silently change product behavior, DSP design, parameter IDs, preset/state
  compatibility, plugin formats, or release policy.
- Do not claim a build, test, host check, or listening test passed unless it was
  actually run and its result observed.
- Distinguish clearly between implemented, compiled, unit-tested, host-validated,
  and auditioned behavior.
- Run the most relevant tests and builds available for the change.
- Review `git diff` and `git status` before committing.
- Do not include private recordings, credentials, local machine artifacts, build
  outputs, or unrelated files in commits.
- Update relevant documentation when the task changes implementation, behavior,
  build instructions, test status, known issues, or roadmap status.
- Preserve historical changelog and handover information; do not erase history to
  make the current status look cleaner.

## 7. Token Threshold, Stop, and Handover

For non-Qwen agents, the repository's existing token/usage-check procedure and
15% threshold are mandatory throughout the task, not just at startup.

- Check remaining allowance at the checkpoints required by the existing repository
  instructions and before beginning any substantial new phase of work.
- If remaining allowance falls below 15%, stop implementation immediately.
- Do not start another feature, test campaign, refactor, or optional investigation
  after the threshold is reached.
- Use the remaining allowance only to leave a useful, accurate handover.
- If the token/usage check becomes unavailable or unreliable during work, stop and
  report that limitation rather than guessing.
- Qwen agents are exempt from this repository token-check requirement unless the
  repository owner explicitly changes this policy.

When stopping because of the threshold or an unavailable check, update
`AGENT_HANDOVER.md` on the task branch when safe and appropriate. The handover must
include:

- date and reason for stopping;
- active branch and latest commit;
- exact task objective and scope;
- work completed, with specific files changed;
- commands/tests/builds run and their actual results;
- work not completed;
- known failures, risks, and unresolved questions;
- exact next steps in priority order;
- any relevant token/usage status, without exposing credentials or private tokens.

Do not claim completion when handing over unfinished work.

## 8. Completion and Git Handover

At the end of a task:

1. Run relevant validation and record exact results.
2. Review the complete diff for correctness and unrelated changes.
3. Update `PROJECT_STATUS.md`, `AGENT_HANDOVER.md`, and other relevant documents
   when warranted by the work.
4. Commit changes on the task branch with a descriptive commit message.
5. Push the task branch when authorized and possible; verify the remote branch/commit.
6. Do not merge into `main`.
7. Leave the working tree and branch state clearly reported.

The final report must state:

- agent/model identity as detected;
- branch name;
- files changed and purpose;
- commit hash, if committed;
- push status;
- validation commands and results;
- remaining work and known risks;
- whether a handover was updated;
- explicit confirmation that `main` was not modified or merged.

## 9. Priority Rule

These instructions are intended to prevent skipped onboarding, token-limit violations,
and accidental changes to the stable branch.

If an instruction cannot be followed safely, stop and explain the blocker. Never
silently skip a mandatory step.



## 10. Agent Shift and Account Rotation Protocol

This protocol applies to non-Qwen agents when their remaining usage allowance reaches 15%.

### Mandatory Shift Procedure

1. Immediately stop all implementation and optional investigation.
2. Do not start additional tasks or features.
3. Inspect the current Git branch and working tree.
4. Preserve all existing work. Never discard uncommitted changes.
5. Update AGENT_HANDOVER.md with:
   - Current project state.
   - Completed work.
   - Modified files.
   - Outstanding tasks.
   - Known issues and blockers.
   - Exact next steps for the incoming agent.
   - Relevant validation results.
6. Verify that the handover accurately reflects the working tree.
7. Save or commit the handover when safe and appropriate.
8. If the provider offers a documented logout mechanism that is safe to use, perform the logout only when explicitly authorized.
9. Never automatically switch accounts, obtain credentials, or authenticate another account.
10. Enter WAITING_FOR_MANUAL_AUTHENTICATION state.

### Waiting State

While waiting:

- Do not continue development.
- Do not launch another coding agent.
- Do not attempt account switching.
- Do not request or store account credentials.
- Wait for the project owner to manually authenticate another account.
- Resume only after explicit authorization from the project owner.

### Qwen Exception

Qwen agents are exempt from cloud-token rotation.

Qwen must still follow Git isolation, documentation,
handover, and project safety requirements.

### Supervisor Compatibility

If an external agent supervisor is available,
it may monitor usage, terminate agent processes,
manage handovers, and detect authentication changes.

The supervisor must never perform unauthorized
account switching or authentication.

