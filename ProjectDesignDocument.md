# Fast Fixed-Point Decimals

## Overview
Database management systems commonly support both floating-point and fixed-point values for storing decimals.
Fixed-point data types in database management systems give predictable results but are commonly slower than floating-point data types.
However, with bit-wise techniques, fixed-point data types can improve their performance significantly.

Our project is to study how fast fixed-point data types are after applying bit-wise techniques in database management systems, PostgreSQL.
More importantly, our project also explores possibilities to make fixed-point data types even faster than floating-point data types.

Since there is an existing open-source project, `libfixeypointy`, we would like to utilize, validate, evaluate, and document the project code.
GMP is used to evaluate the performance of the project.
Accordingly, our project integrates the project with PostgreSQL, a well-known relational database management systems, as a user-defined type.
Our project evaluates the user-defined type to understand the performance after integration against PostgreSQL's built-in `DECIMAL`, which is their fixed-point data type. Furthermore, our project evaluates the user-defined type against PostgreSQL's built-in `REAL`, which is their floating-point data type, to understand a performance gap between fixed-point with bit-wise techniques and floating-point data types in databases.

## Scope
>Which parts of the system will this feature rely on or modify? Write down specifics so people involved can review the design doc

## Glossary (Optional)
>If you are introducing new concepts or giving unintuitive names to components, write them down here.
-

## Architectural Design
>Explain the input and output of the component, describe interactions and breakdown the smaller components if any. Include diagrams if appropriate.

## Design Rationale
>Explain the goals of this design and how the design achieves these goals. Present alternatives considered and document why they are not chosen.

## Testing Plan
>How should the component be tested?

## Trade-offs and Potential Problems
>Write down any conscious trade-off you made that can be problematic in the future, or any problems discovered during the design process that remain unaddressed (technical debts).

## Future Work
>Write down future work to fix known problems or otherwise improve the component.

