# 1Password LD_PRELOAD inotify Workaround - inotify_add_watch

## Background

1Password has a thread/async loop which listens for inotify messages on a number of files, including its own executable.
It calls inotify_add_watch for the `1password` binary with the mask `0xfce` which is `(IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE | IN_MOVE | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF)`. If this inotify event triggers, 1Password will exit. This is to prevent incorrect JavaScript code from being loaded when the application is updated (see https://1password.community/discussion/138445/application-exits-on-rpm-ostree-update).

On a `rpm-ostree` deployment, RPM packages which deploy to `/opt` are actually deployed to `/usr/lib/opt` and a symlink is created in `/var/opt`. The files in `/usr/lib/opt` are immutable because `/usr` is usually mounted read-only. However, on `btrfs` — and likely other filesystems — a read-only mount does not prevent updates to the inode, particularly updates to the link count which is updated when hard links are created (this also resets the ctime). Since `ostree` checks out objects as hard links (similar to git), whenever a new commit is created with duplicate files the link count of `/opt/1Password/1password` can change and cause the program to quit. In practice, 1Password quits whenever `rpm-ostree` is called to upgrade or change packages.

## Workaround

From the background, we know that changes to the file attributes — particularly the link count — is triggering the inotify event which quits 1Password. The workaround is to remove the `IN_ATTRIB` bit from the `mask` provided to `inotify_add_watch()` when the `filepath` ends with `/1password`.

We do this by creating a shared object file which when loaded into the binary with `LD_PRELOAD` or `patchelf` will hook `inotify_add_watch()`.

## Build

```
gcc -Wall -fPIC -shared -o 1p-ldpreload.so 1p-ldpreload.c -ldl
```

## Run

```
LD_PRELOAD=$PWD/1p-ldpreload.so /opt/1Password/1password
```

## Packaging

I'm looking into putting this into an RPM file that can be added to the rpm-ostree overlay and will automatically use `patchelf` to patch the 1password binary.

I will add the RPM spec file here once this is complete.
