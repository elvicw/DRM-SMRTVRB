AGENTS.md — DRM-SMRTVRB
These instructions apply to every AI coding agent working in this repository.
They are mandatory. Do not treat them as suggestions.

The repository owner remains in control of project scope, account authentication,
branch merging, and other actions requiring human approval.
1. Identify the Agent and Select the Startup Path
At the beginning of every task, identify the active model/agent as accurately as
the runtime allows. Do not guess or claim to be Qwen based only on a vague model
family name.

There are two startup paths:
Path A — Qwen
If the active coding model is Qwen (including Qwen 3.5):
1. Before opening or reading any project Markdown documentation, inspect Git
   metadata and the working tree only:
   - git status --short
   - git branch --show-current
   - git log -1 --oneline
2. Do not read README, AGENT_HANDOVER.md, or any other .md file yet.
3. If the working tree has uncommitted changes, STOP. Do not stash, reset, clean,
   discard, overwrite, or otherwise disturb them. Report the state and wait.
4. Create a new task-specific branch and switch to it before reading project
   Markdown files or modifying project files.
5. Verify the active branch with git branch --show-current.
6. Only after the branch is created and verified, continue to Section 3
   (Documentation Understanding).
Qwen agents are exempt from the non-Qwen token-check and cloud-account rotation
requirements unless the repository owner explicitly changes this policy. Qwen
agents are not exempt from Git, documentation, handover, or project safety rules.
Path B — Any non-Qwen agent
If the active model/agent is not Qwen:
1. Before substantial work, locate and read the repository's token/usage-limit
   instructions. Search relevant Markdown documentation for terms such as
   token, usage, quota, context, 15%, handover, and limit.
2. Confirm in the startup report that you understand:
   - how remaining usage allowance is checked;
   - which source or interface provides the value;
   - the stopping threshold (15% or below);
   - what handover is required when the threshold is reached.
3. Actually perform the available usage check before development. Saying that
   you will check later does not satisfy this requirement.
4. If the check is inaccessible, unclear, or unreliable, STOP before development.
   Explain what is unavailable. Do not invent a token API, fabricate a percentage,
   or claim a check passed.
5. Read AGENT_HANDOVER.md and determine whether it describes an unfinished task
   stopped because of token exhaustion or another agent shift.
6. If this is a successor continuing an unfinished handed-over task, follow
   Section 2 (Successor Branch Continuity). Do not create a new branch.
7. Otherwise, if remaining allowance is 15% or below, do not begin implementation.
   Produce the handover described in Section 7 and wait for manual authentication.
8. If the check is valid and remaining allowance is above 15%, create and switch
   to a task-specific branch using Section 2 (New Task Branch Creation).
9. Verify the active branch, then continue to Section 3.
The repository's documented token-check procedure is authoritative. Do not
replace, weaken, bypass, or silently reinterpret it. If instructions conflict,
report the conflict and follow the safer rule: do not begin or continue development
until clarified.
2. Git Branch Management
Every agent must work on a dedicated task branch. No agent may develop directly
on main.
2.1 Successor Branch Continuity — Mandatory
This rule takes precedence over normal new-task branch creation when a non-Qwen
agent is continuing an unfinished task handed over because the previous agent
reached the token threshold or otherwise performed a documented agent shift.

The successor MUST continue the same task on the existing task branch. It MUST
NOT create a new branch for that unfinished task.

Before continuing, the successor must:
1. Read AGENT_HANDOVER.md and identify the unfinished task and its recorded
   active branch.
2. Inspect Git state:
   - git status --short
   - git branch --show-current
   - git log -1 --oneline
   - inspect relevant commits and the diff as needed.
3. Confirm that the branch named in the handover is the branch for the unfinished
   task and that its contents match the handover.
4. Preserve all tracked and untracked changes. Never reset, clean, stash, overwrite,
   discard, or force-checkout existing work.
5. If already on the correct task branch, remain on it.
6. If on another branch and the working tree is clean, switch to the recorded task
   branch and verify it.
7. If the working tree is dirty and the correct task branch is not currently
   checked out, do not switch branches. STOP and report the situation for human
   intervention.
8. If the recorded branch is missing, ambiguous, or inconsistent with the handover,
   STOP and ask the repository owner. Do not create a replacement branch.
9. Once the branch and working state are verified, continue the exact unfinished
   task from the handover. Do not restart completed work unnecessarily.
The successor must not treat an unfinished handover as a new independent task.
Branch continuity takes priority over the normal new-task branch creation procedure.

The handover should identify at least:
- active task branch;
- latest commit hash, if one exists;
- whether changes are committed or uncommitted;
- important modified and untracked files;
- exact completed and incomplete work;
- validation already performed and its results;
- next steps.
Do not assume that a handover is accurate without checking the repository state.
If the handover and Git state disagree, preserve the work and report the mismatch.
2.2 New Task Branch Creation
Use this procedure only when starting a genuinely new, independent task and no
unfinished handover requires continuation.

Choose a short, descriptive branch name based on the assigned task, using a
suitable prefix, for example:
- docs/reconciliation
- fix/space-automation
- feature/stereo-width-control
- test/windows-reaper-validation
- build/release-packaging
Do not use generic names such as work, changes, or agent.

Before creating a branch:
1. Inspect the current branch and working tree.
2. Preserve all existing user or agent changes.
3. If the working tree is not clean, STOP and report it. Never automatically stash,
   reset, clean, checkout over, or discard existing changes.
4. If the repository is not on main, do not force a switch to main. Report the
   current branch and wait for direction unless the task owner explicitly
   instructed otherwise.
5. If on main with a clean working tree, fetch the latest remote information and
   fast-forward local main from origin/main only when safe and possible. If it
   cannot be done safely, stop and report why.
6. Create a new task-specific branch from the updated main, then switch to it.
7. Verify the branch name before editing any project file.
If the intended branch name already exists, do not overwrite, reset, or repurpose
it. Inspect it and choose a distinct task-specific name, or stop if its state is
unclear.
2.3 Git Restrictions
- Never commit directly to main.
- Never merge into main.
- Never force-push.
- Never delete branches or rewrite Git history unless the repository owner
  explicitly authorizes that specific action.
- Do not overwrite another agent's branch or work.
- Push the task branch only when the environment is authenticated and pushing is
  appropriate for the task. Clearly report if it was not pushed.
- Leave the repository on the task branch at the end of the task or handover.
- Human approval is required before merging.
3. Documentation Understanding — Mandatory
After completing the applicable startup path and verifying the correct task branch,
read and understand the repository's Markdown documentation before implementation.
1. Discover all Markdown files in the repository, including root-level and nested
   .md files. Do not rely only on a hard-coded list.
2. Read all project-relevant Markdown files in full where practical. At minimum,
   inspect the complete contents of every project specification, status, roadmap,
   build, testing, issue, handover, and agent-instruction document.
3. Do not assume that reading README.md alone is sufficient.
4. Do not skip a document because it is long, appears repetitive, or seems unrelated
   based only on its filename.
5. If a file is exceptionally large, read it in manageable sections until all
   relevant content has been covered. State any file that could not be read and why.
6. Identify each document's purpose and note contradictions, superseded
   instructions, unresolved decisions, and stale status claims.
7. Treat documentation as project guidance, not proof that a feature exists or works.
After reading the documentation, inspect source code, tests, build configuration,
and relevant CI workflows needed for the assigned task. Source code establishes
what is implemented; actual test/build output establishes what has been verified.
4. Confirm Understanding Before Implementation
Before changing implementation files, provide a concise task-start summary that
demonstrates understanding of:
- the product's purpose and intended workflow;
- relevant architecture and component boundaries;
- relevant DSP/audio behavior and parameter relationships, when applicable;
- UI specification and interaction conventions, when applicable;
- build tools, dependencies, and supported targets;
- current implementation and verification status;
- known issues, constraints, and relevant roadmap items;
- exact task scope and files likely to be affected.
Do not invent missing requirements. Ask for clarification when an unresolved
decision materially affects implementation.

Do not begin unrelated refactoring or feature work. Wait for approval when the
task owner has explicitly required an approval checkpoint.
5. Documentation Reconciliation
Documentation reconciliation is required whenever an agent discovers conflicting,
stale, incomplete, or inaccurate project documentation. It is also mandatory when
the assigned task explicitly requests documentation review or reconciliation.

Before changing documentation:
1. Read the relevant Markdown files and inspect the source code, build
   configuration, tests, and CI workflows needed to establish the actual state.
2. Compare documentation claims against implementation and observed build/test
   results.
3. Identify contradictions, duplicated guidance, stale instructions, missing
   details, and claims that cannot currently be verified.
4. Treat source code as evidence of implementation and actual command output as
   evidence of validation. Do not convert an unverified claim into a verified fact.
When reconciling documentation:
- Update relevant Markdown files so they agree on current project status, build
  instructions, architecture, parameters, testing, known issues, and next steps.
- Preserve historical information in the changelog and other historical records.
- Do not delete useful context merely to remove apparent contradictions; clarify
  what is current, historical, and unverified.
- Do not change source code or product behavior as part of a documentation-only
  task.
- Keep PROJECT_STATUS.md aligned with evidence-backed implementation and
  validation status.
- Keep AGENT_HANDOVER.md actionable and specific about completed work,
  outstanding work, blockers, and next steps.
- Update README.md only when its user-facing project description or instructions
  are inaccurate or incomplete.
- Ensure internal file references, commands, paths, and documented parameter names
  match the repository.
Create or update a reconciliation report (for example,
DOCUMENTATION_RECONCILIATION.md) that records:
- Markdown files inspected and files modified;
- contradictions or stale statements found;
- corrections made and the evidence supporting them;
- unresolved discrepancies or decisions requiring human input;
- checks performed and their actual results.
Do not claim that all documentation is reconciled if any relevant file was not
inspected or any material discrepancy remains unresolved. Clearly list exceptions.
6. Development and Validation Rules
- Stay within the assigned task.
- Follow existing architecture, naming, formatting, and testing conventions.
- Do not silently change product behavior, DSP design, parameter IDs, preset/state
  compatibility, plugin formats, or release policy.
- Inspect the existing dependency and build setup before installing or changing
  tools.
- Install missing dependencies or build tools only when they are required for the
  assigned task, compatible with the project, and safe to install in the current
  environment. Prefer documented project instructions and official sources.
- Do not use elevated privileges, alter global machine configuration, or remove
  existing tools without explicit authorization.
- Record what was installed or changed and the commands used.
- Do not claim a build, test, host check, or listening test passed unless it was
  actually run and its result observed.
- Distinguish clearly between implemented, compiled, unit-tested, host-validated,
  and auditioned behavior.
- Run the most relevant tests and builds available for the change.
- Review git diff and git status before committing.
- Do not include private recordings, credentials, local machine artifacts, build
  outputs, or unrelated files in commits.
- Update relevant documentation when the task changes implementation, behavior,
  build instructions, test status, known issues, or roadmap status.
- Preserve historical changelog and handover information; do not erase history to
  make the current status look cleaner.
7. Token Threshold, Stop, and Handover
This section applies to non-Qwen agents. Qwen agents are exempt from this
repository's cloud-token monitoring and account-rotation requirement unless the
repository owner explicitly changes the policy.
7.1 Monitoring and stopping threshold
- Follow the repository's documented token/usage-check procedure throughout the
  task, not just at startup.
- Check remaining allowance at the checkpoints required by the existing repository
  instructions and before beginning any substantial new phase of work.
- When remaining allowance is 15% or below, stop implementation immediately.
- Do not start another feature, test campaign, refactor, or optional investigation
  after the threshold is reached.
- Use the remaining allowance only to leave a useful and accurate handover.
- If the token/usage check becomes unavailable or unreliable during work, stop
  implementation and report the limitation rather than guessing.
- Never invent a token API, fabricate a percentage, or claim a check passed when
  it did not.
7.2 Required handover
When stopping because of the threshold, an unavailable usage check, or an agent
shift, update AGENT_HANDOVER.md on the existing task branch when safe and
appropriate. Do not create a new branch for the handover.

The handover must include:
- date and reason for stopping;
- agent/model identity, as detected;
- exact task objective and scope;
- active task branch name;
- latest commit hash, if one exists;
- whether the working tree is clean or has uncommitted changes;
- modified, added, and untracked files relevant to the task;
- work completed, with specific files changed;
- commands, tests, builds, and checks run, with actual results;
- work not completed;
- known failures, risks, and unresolved questions;
- exact next steps in priority order;
- relevant token/usage status, without exposing credentials or private tokens;
- explicit instruction: CONTINUE ON THE EXISTING BRANCH — DO NOT CREATE A NEW BRANCH.
Do not claim completion when handing over unfinished work.
7.3 Preserve Git state during handover
1. Stop implementation and optional investigation.
2. Inspect the current branch and working tree.
3. Preserve all tracked, untracked, staged, and unstaged work.
4. Do not reset, clean, stash, discard, overwrite, or force-checkout changes.
5. Update AGENT_HANDOVER.md with the exact state and next steps.
6. Commit the handover and other changes only when doing so is safe and appropriate.
   Do not make a commit that would lose, omit, or misrepresent unfinished work.
7. Verify that the handover accurately reflects the branch, latest commit, and
   working tree.
8. Leave the repository on the existing task branch.
The incoming non-Qwen agent must follow Section 2.1 and resume on this same branch.
If safe continuation is impossible, the incoming agent must stop and request human
intervention rather than creating another branch.
7.4 Account and waiting protocol
- If the provider offers a documented logout mechanism that is safe to use, perform
  logout only when explicitly authorized by the repository owner.
- Never automatically switch accounts, obtain credentials, or authenticate another
  account.
- After preparing and verifying the handover, enter
  WAITING_FOR_MANUAL_AUTHENTICATION.
- While waiting, do not continue development, launch another coding agent, attempt
  account switching, or request/store account credentials.
- Wait for the project owner to manually authenticate another account.
- Resume only after explicit authorization from the project owner.
Qwen agents are exempt from cloud-token rotation, but must still follow Git
isolation, successor branch continuity when applicable, documentation, handover,
and project safety requirements.

An external agent supervisor may monitor usage, terminate agent processes, manage
handovers, and detect authentication changes. It must never perform unauthorized
account switching or authentication.
8. Completion and Git Handover
At the end of a task:
1. Run relevant validation and record exact results.
2. Review the complete diff for correctness and unrelated changes.
3. Update PROJECT_STATUS.md, AGENT_HANDOVER.md, and other relevant documents
   when warranted by the work.
4. Commit changes on the task branch with a descriptive commit message when safe
   and appropriate.
5. Push the task branch when authorized and possible; verify the remote branch and
   commit. Clearly report if it was not pushed.
6. Do not merge into main.
7. Leave the working tree and branch state clearly reported.
The final report must state:
- agent/model identity as detected;
- branch name;
- files changed and their purpose;
- commit hash, if committed;
- push status;
- validation commands and results;
- remaining work and known risks;
- whether AGENT_HANDOVER.md was updated;
- explicit confirmation that main was not modified or merged.
9. Priority and Safety Rule
These instructions are intended to prevent skipped onboarding, token-limit
violations, lost work, unnecessary branch fragmentation, and accidental changes
to the stable branch.

When rules appear to conflict:
1. Preserve existing user and agent work.
2. For an unfinished handed-over task, preserve branch continuity.
3. Do not bypass token limits or authentication safeguards.
4. Do not modify main or merge without human approval.
5. If the safe course is unclear, stop and explain the blocker.
If an instruction cannot be followed safely, stop and explain why. Never silently
skip a mandatory step.