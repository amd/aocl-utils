# 2. Create backward compatibility layer

Date: 2024-05-21

## Context

Inorder to support the legacy apis, currently we have few symlinks created from
newer headers to older headers. The symbolic links are causing problems like,
1. Requires admin privilage to get created in windows.
2. People copying the content of the installation folder ends up with broken
   links. etc.
## Decision

Introduce a Backward compatibility layer(Bcl) and have real files instead of
symlinks and copy them during installation.
## Consequences

Mitigates all the problems of creating a symbolic link during installation.

## Status

Accepted
