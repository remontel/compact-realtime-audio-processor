# Contributing

This project uses a simple GitHub workflow:

**Issue → Branch → Pull Request → Review → Merge**

`main` should always represent the best known integrated version of the project.

---

## Before Starting Work

For substantial work:

1. Create or choose a GitHub Issue.
2. Make sure the scope of the task is clear.
3. Update your local `main` branch.
4. Create a new branch for the task.

Branches represent work, not individual team members.

**Do not work directly on `main`.**

---

## Branch Naming

Use a short category followed by the GitHub Issue number and a brief
description.

Format:

```text
category/issue-number-description
```

Example:

```
dsp/18-digital-delay
```

### Recommended Branch Categories

| Category    | Use                                                          |
| ----------- | ------------------------------------------------------------ |
| `feat/`     | New general functionality that does not fit a more specific category |
| `fix/`      | Bug fixes or corrections to existing work                    |
| `dsp/`      | DSP algorithms, effects, audio processing, or mixing         |
| `ui/`       | Display, controls, encoders, buttons, or user-interface work |
| `hardware/` | Analog circuitry, power, codec support circuitry, amplifier, or other electrical hardware |
| `pcb/`      | KiCad schematics, PCB layout, footprints, routing, or manufacturing preparation |
| `test/`     | Test procedures, validation code, measurements, or test documentation |
| `docs/`     | Documentation-only changes                                   |
| `chore/`    | Repository maintenance, configuration, tooling, or cleanup that does not change system behavior |

Examples:

```
feat/12-audio-passthrough
dsp/18-digital-delay
ui/22-oled-driver
fix/27-dma-underrun
hardware/19-mic-preamp
pcb/24-codec-breakout
test/31-latency-measurement
docs/8-update-architecture
chore/5-update-gitignore
```

These categories are intended to make branches easier to understand, not to create rigid ownership boundaries.

## Working on a Branch

Keep each branch focused on one task.

Avoid including unrelated cleanup or changes in the same branch.

Make commits at logical checkpoints and use descriptive commit messages.

Good:

```
Add initial OLED SPI driver
```

Avoid:

```
stuff
```

---

## Example Workflow

The following example starts work on GitHub Issue #18 for the digital delay.

#### 1. Update `main`

```
git switch main
git pull
```

This makes sure your new branch starts from the latest integrated version of the project.

#### 2. Create a branch

```
git switch -c dsp/18-digital-delay
```

You are now working on a separate branch without changing `main`.

#### 3. Make your changes

Check which files have changed:

```
git status
```

Review the changes before committing:

```
git diff
```

#### 4. Stage the files you want to commit

For specific files:

```
git add firmware/
```

Or, if you have reviewed everything and want to stage all current changes:

```
git add .
```

#### 5. Commit the change

```
git commit -m "Add initial digital delay implementation"
```

#### 6. Push the branch to GitHub

```
git push -u origin dsp/18-digital-delay
```

The `-u` connects your local branch to the corresponding branch on GitHub.
After this first push, future pushes from the same branch can normally use:

```
git push
```

#### 7. Open a Pull Request

On GitHub:

1. Open the repository.
2. Select the newly pushed branch.
3. Choose **Compare & pull request**.
4. Fill out the Pull Request template.
5. Link the related Issue.
6. Request review from at least one teammate.

The PR should explain:

- what changed
- why it changed
- the related GitHub Issue
- how it was tested
- anything that still needs hardware validation

#### 8. Review

**Every Pull Request must be reviewed and approved by at least one other team
member before it is merged.**

The author should not approve their own PR.

For larger changes involving PCB design, power, system architecture, component selection, or subsystem interfaces, ask for additional review from teammates who are affected by the change.

Reviewers should check both the implementation and the engineering reasoning where appropriate.

#### 9. Merge

After the PR has been approved and any review comments have been resolved, use:

**Squash and Merge**

This keeps `main` readable by combining the branch's work into one logical commit.

#### 10. Clean up

After the PR is merged, delete the remote branch through GitHub.

Then update your local repository:

```
git switch main
git pull
```

Delete the old local branch:

```
git branch -d dsp/18-digital-delay
```

You are now ready to create a new branch for the next task.

---

## Pull Requests

Every PR must receive approval from **at least one teammate other than the author** before merging.

Use the repository Pull Request template.

A PR should include:

- a short summary of the change
- the related GitHub Issue
- the engineering reason for the change when relevant
- tests or validation performed
- known limitations
- hardware validation that still needs to be performed
- documentation that was updated

If the work is still in progress but would benefit from discussion, open a **Draft Pull Request**.

---

## Git Clients

You may use the Git command line or a graphical Git client such as GitKraken.

The same workflow applies regardless of the tool:

1. Update `main`
2. Create a branch
3. Make changes
4. Commit
5. Push
6. Open a Pull Request
7. Get approval from at least one teammate
8. Merge

GitKraken, GitHub Desktop, VS Code, and the command line are all acceptable as long as the repository workflow is followed.

---

## Engineering Changes

When making changes that affect system architecture, hardware interfaces, requirements, or other subsystems, update the relevant documentation under `docs/`.

Significant architectural decisions should be recorded in [`docs/decisions/`](docs/decisions/).

Do not guess component specifications, electrical requirements, pinouts, or interface details. Verify them against the relevant datasheet or project documentation.

---

## Testing

Only report tests that were actually performed.

Clearly distinguish between work that is:

- implemented
- compiled
- simulated
- bench tested
- hardware verified
- still untested
