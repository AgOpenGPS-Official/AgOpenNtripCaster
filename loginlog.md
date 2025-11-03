[23:09:45 DBG] Registered model binder providers, in the following order: ["Microsoft.AspNetCore.Mvc.ModelBinding.Binders.BinderTypeModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.ServicesModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.BodyModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.HeaderModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.FloatingPointTypeModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.EnumTypeModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.DateTimeModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.SimpleTypeModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.TryParseModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.CancellationTokenModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.ByteArrayModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.FormFileModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.FormCollectionModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.KeyValuePairModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.DictionaryModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.ArrayModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.CollectionModelBinderProvider", "Microsoft.AspNetCore.Mvc.ModelBinding.Binders.ComplexObjectModelBinderProvider"]
[23:09:45 DBG] Registered SignalR Protocol: json, implemented by Microsoft.AspNetCore.SignalR.Protocol.JsonHubProtocol.
[23:09:47 DBG] An 'IServiceProvider' was created for internal use by Entity Framework.
[23:09:48 INF] User profile is available. Using 'C:\Users\hp\AppData\Local\ASP.NET\DataProtection-Keys' as key repository and Windows DPAPI to encrypt keys at rest.
[23:09:49 DBG] The index {'UserId'} was not created on entity type 'IdentityUserRole<string>' as the properties are already covered by the index {'UserId', 'RoleId'}.
[23:09:49 DBG] The index {'UserId'} was not created on entity type 'IdentityUserToken<string>' as the properties are already covered by the index {'UserId', 'LoginProvider', 'Name'}.
[23:09:49 DBG] The index {'AllowedGroupsId'} was not created on entity type 'MountPointNtripGroup (Dictionary<string, object>)' as the properties are already covered by the index {'AllowedGroupsId', 'MountPointsId'}.
[23:09:49 DBG] The index {'GroupsId'} was not created on entity type 'NtripGroupNtripUser (Dictionary<string, object>)' as the properties are already covered by the index {'GroupsId', 'UsersId'}.
[23:09:49 DBG] The property 'MountPoint.OwnerId' was created in shadow state because there are no eligible CLR members with a matching name.
[23:09:49 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:09:49 DBG] Creating DbConnection.
[23:09:49 DBG] Created DbConnection. (71ms).
[23:09:49 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:50 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:50 DBG] Creating DbCommand for 'ExecuteReader'.
[23:09:50 DBG] Created DbCommand for 'ExecuteReader' (12ms).
[23:09:50 DBG] Initialized DbCommand for 'ExecuteReader' (25ms).
[23:09:50 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT "MigrationId", "ProductVersion"
FROM "__EFMigrationsHistory"
ORDER BY "MigrationId";
[23:09:50 INF] Executed DbCommand (64ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT "MigrationId", "ProductVersion"
FROM "__EFMigrationsHistory"
ORDER BY "MigrationId";
[23:09:50 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:50 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 72ms reading results.
[23:09:50 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:50 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (18ms).
[23:09:50 DBG] Migrating using database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:50 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:50 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:50 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:50 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:50 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (8ms).
[23:09:50 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:50 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:50 DBG] Creating DbCommand for 'ExecuteNonQuery'.
[23:09:50 DBG] Created DbCommand for 'ExecuteNonQuery' (2ms).
[23:09:50 DBG] Initialized DbCommand for 'ExecuteNonQuery' (5ms).
[23:09:50 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
CREATE TABLE IF NOT EXISTS "__EFMigrationsHistory" (
    "MigrationId" character varying(150) NOT NULL,
    "ProductVersion" character varying(32) NOT NULL,
    CONSTRAINT "PK___EFMigrationsHistory" PRIMARY KEY ("MigrationId")
);
[23:09:50 INF] Executed DbCommand (16ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
CREATE TABLE IF NOT EXISTS "__EFMigrationsHistory" (
    "MigrationId" character varying(150) NOT NULL,
    "ProductVersion" character varying(32) NOT NULL,
    CONSTRAINT "PK___EFMigrationsHistory" PRIMARY KEY ("MigrationId")
);
[23:09:50 DBG] Beginning transaction with isolation level 'Unspecified'.
[23:09:50 DBG] Began transaction with isolation level 'ReadCommitted'.
[23:09:50 DBG] Creating DbCommand for 'ExecuteNonQuery'.
[23:09:50 DBG] Created DbCommand for 'ExecuteNonQuery' (7ms).
[23:09:50 DBG] Initialized DbCommand for 'ExecuteNonQuery' (16ms).
[23:09:50 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
LOCK TABLE "__EFMigrationsHistory" IN ACCESS EXCLUSIVE MODE
[23:09:51 INF] Executed DbCommand (16ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
LOCK TABLE "__EFMigrationsHistory" IN ACCESS EXCLUSIVE MODE
[23:09:51 DBG] Creating DbCommand for 'ExecuteReader'.
[23:09:51 DBG] Created DbCommand for 'ExecuteReader' (8ms).
[23:09:51 DBG] Initialized DbCommand for 'ExecuteReader' (33ms).
[23:09:51 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT "MigrationId", "ProductVersion"
FROM "__EFMigrationsHistory"
ORDER BY "MigrationId";
[23:09:51 INF] Executed DbCommand (41ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT "MigrationId", "ProductVersion"
FROM "__EFMigrationsHistory"
ORDER BY "MigrationId";
[23:09:51 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:51 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 17ms reading results.
[23:09:51 INF] No migrations were applied. The database is already up to date.
[23:09:51 DBG] Committing transaction.
[23:09:51 DBG] Committed transaction.
[23:09:51 DBG] Disposing transaction.
[23:09:51 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:51 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (12ms).
[23:09:51 INF] Database migrated successfully
[23:09:51 DBG] Compiling query expression:
'DbSet<NtripUser>()
    .Any()'
[23:09:51 DBG] Generated query execution expression:
'queryContext => SingleQueryingEnumerable.Create<bool>(
    relationalQueryContext: (RelationalQueryContext)queryContext,
    relationalCommandResolver: parameters => [LIFTABLE Constant: RelationalCommandCache.QueryExpression(
        Projection Mapping:
            EmptyProjectionMember -> 0
        SELECT EXISTS (
            SELECT 1
            FROM AspNetUsers AS a)) | Resolver: c => new RelationalCommandCache(
        c.Dependencies.MemoryCache,
        c.RelationalDependencies.QuerySqlGeneratorFactory,
        c.RelationalDependencies.RelationalParameterBasedSqlProcessorFactory,
        Projection Mapping:
            EmptyProjectionMember -> 0
        SELECT EXISTS (
            SELECT 1
            FROM AspNetUsers AS a),
        False,
        new HashSet<string>(
            new string[]{ },
            StringComparer.Ordinal
        )
    )].GetRelationalCommandTemplate(parameters),
    readerColumns: null,
    shaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
    {
        bool? value1;
        value1 = dataReader.IsDBNull(0) ? default(bool?) : (bool?)dataReader.GetBoolean(0);
        return (bool)value1;
    },
    contextType: AgOpenNtripCaster.Server.Data.ApplicationDbContext,
    standAloneStateManager: False,
    detailedErrorsEnabled: False,
    threadSafetyChecksEnabled: True)
    .Single()'
[23:09:51 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:51 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:51 DBG] Creating DbCommand for 'ExecuteReader'.
[23:09:51 DBG] Created DbCommand for 'ExecuteReader' (8ms).
[23:09:51 DBG] Initialized DbCommand for 'ExecuteReader' (20ms).
[23:09:51 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT EXISTS (
    SELECT 1
    FROM "AspNetUsers" AS a)
[23:09:51 INF] Executed DbCommand (24ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT EXISTS (
    SELECT 1
    FROM "AspNetUsers" AS a)
[23:09:51 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:51 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 16ms reading results.
[23:09:51 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:51 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (10ms).
[23:09:51 INF] Database already seeded, skipping seeding
[23:09:51 DBG] 'ApplicationDbContext' disposed.
[23:09:51 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:51 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (14ms).
[23:09:51 INF] Starting NtripCaster server...
[23:09:51 DBG] Hosting starting
[23:09:51 DBG] Reading data from file 'C:\Users\hp\AppData\Local\ASP.NET\DataProtection-Keys\key-0d5b9727-093e-4e02-961b-8ee49a11dc26.xml'.
[23:09:51 DBG] Reading data from file 'C:\Users\hp\AppData\Local\ASP.NET\DataProtection-Keys\key-a278faa2-cc40-4baf-9147-39f10a06d5fd.xml'.
[23:09:52 DBG] Found key {0d5b9727-093e-4e02-961b-8ee49a11dc26}.
[23:09:52 DBG] Found key {a278faa2-cc40-4baf-9147-39f10a06d5fd}.
[23:09:52 DBG] Considering key {0d5b9727-093e-4e02-961b-8ee49a11dc26} with expiration date 2025-11-19 13:16:45Z as default key.
[23:09:52 DBG] Forwarded activator type request from Microsoft.AspNetCore.DataProtection.XmlEncryption.DpapiXmlDecryptor, Microsoft.AspNetCore.DataProtection, Version=9.0.0.0, Culture=neutral, PublicKeyToken=adb9793829ddae60 to Microsoft.AspNetCore.DataProtection.XmlEncryption.DpapiXmlDecryptor, Microsoft.AspNetCore.DataProtection, Culture=neutral, PublicKeyToken=adb9793829ddae60
[23:09:52 DBG] Decrypting secret element using Windows DPAPI.
[23:09:52 DBG] Forwarded activator type request from Microsoft.AspNetCore.DataProtection.AuthenticatedEncryption.ConfigurationModel.AuthenticatedEncryptorDescriptorDeserializer, Microsoft.AspNetCore.DataProtection, Version=9.0.0.0, Culture=neutral, PublicKeyToken=adb9793829ddae60 to Microsoft.AspNetCore.DataProtection.AuthenticatedEncryption.ConfigurationModel.AuthenticatedEncryptorDescriptorDeserializer, Microsoft.AspNetCore.DataProtection, Culture=neutral, PublicKeyToken=adb9793829ddae60
[23:09:52 DBG] Opening CNG algorithm 'AES' from provider 'null' with chaining mode CBC.
[23:09:52 DBG] Opening CNG algorithm 'SHA256' from provider 'null' with HMAC.
[23:09:52 DBG] Using key {0d5b9727-093e-4e02-961b-8ee49a11dc26} as the default key.
[23:09:52 DBG] Key ring with default key {0d5b9727-093e-4e02-961b-8ee49a11dc26} was loaded during application startup.
[23:09:52 INF] Starting NTRIP Server on port 2101...
[23:09:52 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:09:52 DBG] Compiling query expression:
'DbSet<ClientSession>()
    .Where(cs => cs.DisconnectedAt == null)'
[23:09:52 DBG] Generated query execution expression:
'queryContext => SingleQueryingEnumerable.Create<ClientSession>(
    relationalQueryContext: (RelationalQueryContext)queryContext,
    relationalCommandResolver: parameters => [LIFTABLE Constant: RelationalCommandCache.QueryExpression(
        Projection Mapping:
            EmptyProjectionMember -> Dictionary<IProperty, int> { [Property: ClientSession.Id (string) Required PK AfterSave:Throw, 0], [Property: ClientSession.BytesReceived (long) Required, 1], [Property: ClientSession.BytesSent (long) Required, 2], [Property: ClientSession.ClientIpAddress (string), 3], [Property: ClientSession.ConnectedAt (DateTime) Required, 4], [Property: ClientSession.DisconnectedAt (DateTime?), 5], [Property: ClientSession.LastAccuracy (double?), 6], [Property: ClientSession.LastLatitude (double?), 7], [Property: ClientSession.LastLongitude (double?), 8], [Property: ClientSession.LastPositionAt (DateTime?), 9], [Property: ClientSession.LastStreamPauseAt (DateTime?), 10], [Property: ClientSession.MountPointId (int) Required FK Index, 11], [Property: ClientSession.SerialNumber (int) Required, 12], [Property: ClientSession.Status (ClientStreamStatus) Required, 13], [Property: ClientSession.UserId (string) Required FK Index, 14] }
        SELECT c.Id, c.BytesReceived, c.BytesSent, c.ClientIpAddress, c.ConnectedAt, c.DisconnectedAt, c.LastAccuracy, c.LastLatitude, c.LastLongitude, c.LastPositionAt, c.LastStreamPauseAt, c.MountPointId, c.SerialNumber, c.Status, c.UserId
        FROM ClientSessions AS c
        WHERE c.DisconnectedAt == NULL) | Resolver: c => new RelationalCommandCache(
        c.Dependencies.MemoryCache,
        c.RelationalDependencies.QuerySqlGeneratorFactory,
        c.RelationalDependencies.RelationalParameterBasedSqlProcessorFactory,
        Projection Mapping:
            EmptyProjectionMember -> Dictionary<IProperty, int> { [Property: ClientSession.Id (string) Required PK AfterSave:Throw, 0], [Property: ClientSession.BytesReceived (long) Required, 1], [Property: ClientSession.BytesSent (long) Required, 2], [Property: ClientSession.ClientIpAddress (string), 3], [Property: ClientSession.ConnectedAt (DateTime) Required, 4], [Property: ClientSession.DisconnectedAt (DateTime?), 5], [Property: ClientSession.LastAccuracy (double?), 6], [Property: ClientSession.LastLatitude (double?), 7], [Property: ClientSession.LastLongitude (double?), 8], [Property: ClientSession.LastPositionAt (DateTime?), 9], [Property: ClientSession.LastStreamPauseAt (DateTime?), 10], [Property: ClientSession.MountPointId (int) Required FK Index, 11], [Property: ClientSession.SerialNumber (int) Required, 12], [Property: ClientSession.Status (ClientStreamStatus) Required, 13], [Property: ClientSession.UserId (string) Required FK Index, 14] }
        SELECT c.Id, c.BytesReceived, c.BytesSent, c.ClientIpAddress, c.ConnectedAt, c.DisconnectedAt, c.LastAccuracy, c.LastLatitude, c.LastLongitude, c.LastPositionAt, c.LastStreamPauseAt, c.MountPointId, c.SerialNumber, c.Status, c.UserId
        FROM ClientSessions AS c
        WHERE c.DisconnectedAt == NULL,
        False,
        new HashSet<string>(
            new string[]{ },
            StringComparer.Ordinal
        )
    )].GetRelationalCommandTemplate(parameters),
    readerColumns: null,
    shaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
    {
        ClientSession entity;
        entity =
        {
            MaterializationContext materializationContext1;
            IEntityType entityType1;
            ClientSession instance1;
            InternalEntityEntry entry1;
            bool hasNullKey1;
            materializationContext1 = new MaterializationContext(
                [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                queryContext.Context
            );
            instance1 = default(ClientSession);
            entry1 = queryContext.TryGetEntry(
                key: [LIFTABLE Constant: Key: ClientSession.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.ClientSession").FindPrimaryKey()],
                keyValues: new object[]{ (object)dataReader.GetString(0) },
                throwOnNullKey: True,
                hasNullKey: hasNullKey1);
            !(hasNullKey1) ? entry1 != default(InternalEntityEntry) ?
            {
                entityType1 = entry1.EntityType;
                return instance1 = (ClientSession)entry1.Entity;
            } :
            {
                ISnapshot shadowSnapshot1;
                shadowSnapshot1 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                entityType1 = [LIFTABLE Constant: EntityType: ClientSession | Resolver: namelessParameter{0} => namelessParameter{0}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.ClientSession")];
                instance1 = switch (entityType1)
                {
                    case [LIFTABLE Constant: EntityType: ClientSession | Resolver: namelessParameter{1} => namelessParameter{1}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.ClientSession")]:
                        {
                            return
                            {
                                ClientSession instance;
                                instance = new ClientSession();
                                instance.<Id>k__BackingField = dataReader.GetString(0);
                                instance.<BytesReceived>k__BackingField = dataReader.GetInt64(1);
                                instance.<BytesSent>k__BackingField = dataReader.GetInt64(2);
                                instance.<ClientIpAddress>k__BackingField = dataReader.IsDBNull(3) ? default(string) : dataReader.GetString(3);
                                instance.<ConnectedAt>k__BackingField = dataReader.GetDateTime(4);
                                instance.<DisconnectedAt>k__BackingField = dataReader.IsDBNull(5) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(5);
                                instance.<LastAccuracy>k__BackingField = dataReader.IsDBNull(6) ? default(double?) : (double?)dataReader.GetDouble(6);
                                instance.<LastLatitude>k__BackingField = dataReader.IsDBNull(7) ? default(double?) : (double?)dataReader.GetDouble(7);
                                instance.<LastLongitude>k__BackingField = dataReader.IsDBNull(8) ? default(double?) : (double?)dataReader.GetDouble(8);
                                instance.<LastPositionAt>k__BackingField = dataReader.IsDBNull(9) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(9);
                                instance.<LastStreamPauseAt>k__BackingField = dataReader.IsDBNull(10) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(10);
                                instance.<MountPointId>k__BackingField = dataReader.GetInt32(11);
                                instance.<SerialNumber>k__BackingField = dataReader.GetInt32(12);
                                instance.<Status>k__BackingField = Invoke(((EnumToNumberConverter<ClientStreamStatus, int>)((IReadOnlyProperty)[LIFTABLE Constant: Property: ClientSession.Status (ClientStreamStatus) Required | Resolver: namelessParameter{2} => namelessParameter{2}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.ClientSession").FindProperty("Status")]).GetTypeMapping().Converter).ConvertFromProviderTyped, dataReader.GetInt32(13));
                                instance.<UserId>k__BackingField = dataReader.GetString(14);
                                (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                    context: materializationContext1.Context,
                                    entity: instance,
                                    queryTrackingBehavior: TrackAll,
                                    structuralType: [LIFTABLE Constant: EntityType: ClientSession | Resolver: namelessParameter{3} => namelessParameter{3}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.ClientSession")]) : default(void);
                                return instance;
                            }}
                    default:
                        default(ClientSession)
                }
                ;
                entry1 = entityType1 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                    entityType: entityType1,
                    entity: instance1,
                    snapshot: shadowSnapshot1);
                return instance1;
            } : default(void);
            return instance1;
        };
        return entity;
    },
    contextType: AgOpenNtripCaster.Server.Data.ApplicationDbContext,
    standAloneStateManager: False,
    detailedErrorsEnabled: False,
    threadSafetyChecksEnabled: True)'
[23:09:52 DBG] Creating DbConnection.
[23:09:52 DBG] Created DbConnection. (2ms).
[23:09:52 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:52 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:52 DBG] Creating DbCommand for 'ExecuteReader'.
[23:09:52 DBG] Created DbCommand for 'ExecuteReader' (5ms).
[23:09:52 DBG] Initialized DbCommand for 'ExecuteReader' (9ms).
[23:09:52 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:09:52 INF] Executed DbCommand (16ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:09:52 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:52 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 12ms reading results.
[23:09:52 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:52 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (3ms).
[23:09:52 INF] ? No orphaned ClientSessions to clean up
[23:09:52 DBG] Compiling query expression:
'DbSet<SourceConnection>()
    .Where(sc => sc.DisconnectedAt == null)'
[23:09:52 DBG] Generated query execution expression:
'queryContext => SingleQueryingEnumerable.Create<SourceConnection>(
    relationalQueryContext: (RelationalQueryContext)queryContext,
    relationalCommandResolver: parameters => [LIFTABLE Constant: RelationalCommandCache.QueryExpression(
        Projection Mapping:
            EmptyProjectionMember -> Dictionary<IProperty, int> { [Property: SourceConnection.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 0], [Property: SourceConnection.BytesReceived (long) Required, 1], [Property: SourceConnection.BytesSent (long) Required, 2], [Property: SourceConnection.ConnectedAt (DateTime) Required, 3], [Property: SourceConnection.DisconnectedAt (DateTime?), 4], [Property: SourceConnection.MountPointId (int) Required FK Index, 5], [Property: SourceConnection.Status (SourceConnectionStatus) Required, 6] }
        SELECT s.Id, s.BytesReceived, s.BytesSent, s.ConnectedAt, s.DisconnectedAt, s.MountPointId, s.Status
        FROM SourceConnections AS s
        WHERE s.DisconnectedAt == NULL) | Resolver: c => new RelationalCommandCache(
        c.Dependencies.MemoryCache,
        c.RelationalDependencies.QuerySqlGeneratorFactory,
        c.RelationalDependencies.RelationalParameterBasedSqlProcessorFactory,
        Projection Mapping:
            EmptyProjectionMember -> Dictionary<IProperty, int> { [Property: SourceConnection.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 0], [Property: SourceConnection.BytesReceived (long) Required, 1], [Property: SourceConnection.BytesSent (long) Required, 2], [Property: SourceConnection.ConnectedAt (DateTime) Required, 3], [Property: SourceConnection.DisconnectedAt (DateTime?), 4], [Property: SourceConnection.MountPointId (int) Required FK Index, 5], [Property: SourceConnection.Status (SourceConnectionStatus) Required, 6] }
        SELECT s.Id, s.BytesReceived, s.BytesSent, s.ConnectedAt, s.DisconnectedAt, s.MountPointId, s.Status
        FROM SourceConnections AS s
        WHERE s.DisconnectedAt == NULL,
        False,
        new HashSet<string>(
            new string[]{ },
            StringComparer.Ordinal
        )
    )].GetRelationalCommandTemplate(parameters),
    readerColumns: null,
    shaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
    {
        SourceConnection entity;
        entity =
        {
            MaterializationContext materializationContext1;
            IEntityType entityType1;
            SourceConnection instance1;
            InternalEntityEntry entry1;
            bool hasNullKey1;
            materializationContext1 = new MaterializationContext(
                [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                queryContext.Context
            );
            instance1 = default(SourceConnection);
            entry1 = queryContext.TryGetEntry(
                key: [LIFTABLE Constant: Key: SourceConnection.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection").FindPrimaryKey()],
                keyValues: new object[]{ (object)dataReader.GetInt32(0) },
                throwOnNullKey: True,
                hasNullKey: hasNullKey1);
            !(hasNullKey1) ? entry1 != default(InternalEntityEntry) ?
            {
                entityType1 = entry1.EntityType;
                return instance1 = (SourceConnection)entry1.Entity;
            } :
            {
                ISnapshot shadowSnapshot1;
                shadowSnapshot1 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                entityType1 = [LIFTABLE Constant: EntityType: SourceConnection | Resolver: namelessParameter{0} => namelessParameter{0}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection")];
                instance1 = switch (entityType1)
                {
                    case [LIFTABLE Constant: EntityType: SourceConnection | Resolver: namelessParameter{1} => namelessParameter{1}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection")]:
                        {
                            return
                            {
                                SourceConnection instance;
                                instance = new SourceConnection();
                                instance.<Id>k__BackingField = dataReader.GetInt32(0);
                                instance.<BytesReceived>k__BackingField = dataReader.GetInt64(1);
                                instance.<BytesSent>k__BackingField = dataReader.GetInt64(2);
                                instance.<ConnectedAt>k__BackingField = dataReader.GetDateTime(3);
                                instance.<DisconnectedAt>k__BackingField = dataReader.IsDBNull(4) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(4);
                                instance.<MountPointId>k__BackingField = dataReader.GetInt32(5);
                                instance.<Status>k__BackingField = Invoke(((EnumToNumberConverter<SourceConnectionStatus, int>)((IReadOnlyProperty)[LIFTABLE Constant: Property: SourceConnection.Status (SourceConnectionStatus) Required | Resolver: namelessParameter{2} => namelessParameter{2}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection").FindProperty("Status")]).GetTypeMapping().Converter).ConvertFromProviderTyped, dataReader.GetInt32(6));
                                (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                    context: materializationContext1.Context,
                                    entity: instance,
                                    queryTrackingBehavior: TrackAll,
                                    structuralType: [LIFTABLE Constant: EntityType: SourceConnection | Resolver: namelessParameter{3} => namelessParameter{3}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection")]) : default(void);
                                return instance;
                            }}
                    default:
                        default(SourceConnection)
                }
                ;
                entry1 = entityType1 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                    entityType: entityType1,
                    entity: instance1,
                    snapshot: shadowSnapshot1);
                return instance1;
            } : default(void);
            return instance1;
        };
        return entity;
    },
    contextType: AgOpenNtripCaster.Server.Data.ApplicationDbContext,
    standAloneStateManager: False,
    detailedErrorsEnabled: False,
    threadSafetyChecksEnabled: True)'
[23:09:52 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:52 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:52 DBG] Creating DbCommand for 'ExecuteReader'.
[23:09:52 DBG] Created DbCommand for 'ExecuteReader' (4ms).
[23:09:52 DBG] Initialized DbCommand for 'ExecuteReader' (8ms).
[23:09:52 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:09:52 INF] Executed DbCommand (12ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:09:52 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:52 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 4ms reading results.
[23:09:52 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:52 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (3ms).
[23:09:52 INF] ? No orphaned SourceConnections to clean up
[23:09:52 DBG] 'ApplicationDbContext' disposed.
[23:09:52 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:09:52 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (2ms).
[23:09:52 INF] NTRIP Server started on port 2101
[23:09:52 INF] Connection statistics collection started
[23:09:52 DBG] Middleware loaded
[23:09:52 DBG] Middleware loaded. Script /_framework/aspnetcore-browser-refresh.js (16533 B).
[23:09:52 DBG] Middleware loaded. Script /_framework/blazor-hotreload.js (799 B).
[23:09:52 DBG] Middleware configuration started with options: {AllowedHosts = *, AllowEmptyHosts = True, IncludeFailureMessage = True}
[23:09:52 DBG] Wildcard detected, all requests with hosts will be allowed.
[23:09:52 DBG] Middleware loaded: DOTNET_MODIFIABLE_ASSEMBLIES=debug, __ASPNETCORE_BROWSER_TOOLS=true
[23:09:53 INF] Now listening on: http://localhost:5000
[23:09:53 DBG] Loaded hosting startup assembly AgOpenNtripCaster.Server
[23:09:53 DBG] Loaded hosting startup assembly Microsoft.WebTools.ApiEndpointDiscovery
[23:09:53 DBG] Loaded hosting startup assembly Microsoft.AspNetCore.Watch.BrowserRefresh
[23:09:53 DBG] Loaded hosting startup assembly Microsoft.WebTools.BrowserLink.Net
[23:09:53 DBG] Executing API description provider 'EndpointMetadataApiDescriptionProvider' from assembly Microsoft.AspNetCore.Mvc.ApiExplorer v9.0.0.0.
[23:09:53 DBG] Executing API description provider 'DefaultApiDescriptionProvider' from assembly Microsoft.AspNetCore.Mvc.ApiExplorer v9.0.0.0.
[23:09:53 INF] Application started. Press Ctrl+C to shut down.
[23:09:53 INF] Hosting environment: Development
[23:09:53 INF] Content root path: C:\Users\hp\Documents\GitHub\ntripcaster\AgOpenNtripCaster.Server
[23:09:53 DBG] Hosting started
[23:10:02 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:02 DBG] Compiling query expression:
'DbSet<ClientSession>()
    .AsNoTracking()
    .Where(cs => cs.DisconnectedAt == null)'
[23:10:02 DBG] Generated query execution expression:
'queryContext => SingleQueryingEnumerable.Create<ClientSession>(
    relationalQueryContext: (RelationalQueryContext)queryContext,
    relationalCommandResolver: parameters => [LIFTABLE Constant: RelationalCommandCache.QueryExpression(
        Projection Mapping:
            EmptyProjectionMember -> Dictionary<IProperty, int> { [Property: ClientSession.Id (string) Required PK AfterSave:Throw, 0], [Property: ClientSession.BytesReceived (long) Required, 1], [Property: ClientSession.BytesSent (long) Required, 2], [Property: ClientSession.ClientIpAddress (string), 3], [Property: ClientSession.ConnectedAt (DateTime) Required, 4], [Property: ClientSession.DisconnectedAt (DateTime?), 5], [Property: ClientSession.LastAccuracy (double?), 6], [Property: ClientSession.LastLatitude (double?), 7], [Property: ClientSession.LastLongitude (double?), 8], [Property: ClientSession.LastPositionAt (DateTime?), 9], [Property: ClientSession.LastStreamPauseAt (DateTime?), 10], [Property: ClientSession.MountPointId (int) Required FK Index, 11], [Property: ClientSession.SerialNumber (int) Required, 12], [Property: ClientSession.Status (ClientStreamStatus) Required, 13], [Property: ClientSession.UserId (string) Required FK Index, 14] }
        SELECT c.Id, c.BytesReceived, c.BytesSent, c.ClientIpAddress, c.ConnectedAt, c.DisconnectedAt, c.LastAccuracy, c.LastLatitude, c.LastLongitude, c.LastPositionAt, c.LastStreamPauseAt, c.MountPointId, c.SerialNumber, c.Status, c.UserId
        FROM ClientSessions AS c
        WHERE c.DisconnectedAt == NULL) | Resolver: c => new RelationalCommandCache(
        c.Dependencies.MemoryCache,
        c.RelationalDependencies.QuerySqlGeneratorFactory,
        c.RelationalDependencies.RelationalParameterBasedSqlProcessorFactory,
        Projection Mapping:
            EmptyProjectionMember -> Dictionary<IProperty, int> { [Property: ClientSession.Id (string) Required PK AfterSave:Throw, 0], [Property: ClientSession.BytesReceived (long) Required, 1], [Property: ClientSession.BytesSent (long) Required, 2], [Property: ClientSession.ClientIpAddress (string), 3], [Property: ClientSession.ConnectedAt (DateTime) Required, 4], [Property: ClientSession.DisconnectedAt (DateTime?), 5], [Property: ClientSession.LastAccuracy (double?), 6], [Property: ClientSession.LastLatitude (double?), 7], [Property: ClientSession.LastLongitude (double?), 8], [Property: ClientSession.LastPositionAt (DateTime?), 9], [Property: ClientSession.LastStreamPauseAt (DateTime?), 10], [Property: ClientSession.MountPointId (int) Required FK Index, 11], [Property: ClientSession.SerialNumber (int) Required, 12], [Property: ClientSession.Status (ClientStreamStatus) Required, 13], [Property: ClientSession.UserId (string) Required FK Index, 14] }
        SELECT c.Id, c.BytesReceived, c.BytesSent, c.ClientIpAddress, c.ConnectedAt, c.DisconnectedAt, c.LastAccuracy, c.LastLatitude, c.LastLongitude, c.LastPositionAt, c.LastStreamPauseAt, c.MountPointId, c.SerialNumber, c.Status, c.UserId
        FROM ClientSessions AS c
        WHERE c.DisconnectedAt == NULL,
        False,
        new HashSet<string>(
            new string[]{ },
            StringComparer.Ordinal
        )
    )].GetRelationalCommandTemplate(parameters),
    readerColumns: null,
    shaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
    {
        ClientSession entity;
        entity =
        {
            MaterializationContext materializationContext1;
            IEntityType entityType1;
            ClientSession instance1;
            materializationContext1 = new MaterializationContext(
                [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                queryContext.Context
            );
            instance1 = default(ClientSession);
            (object)dataReader.GetString(0) != null ?
            {
                ISnapshot shadowSnapshot1;
                shadowSnapshot1 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                entityType1 = [LIFTABLE Constant: EntityType: ClientSession | Resolver: namelessParameter{0} => namelessParameter{0}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.ClientSession")];
                instance1 = switch (entityType1)
                {
                    case [LIFTABLE Constant: EntityType: ClientSession | Resolver: namelessParameter{1} => namelessParameter{1}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.ClientSession")]:
                        {
                            return
                            {
                                ClientSession instance;
                                instance = new ClientSession();
                                instance.<Id>k__BackingField = dataReader.GetString(0);
                                instance.<BytesReceived>k__BackingField = dataReader.GetInt64(1);
                                instance.<BytesSent>k__BackingField = dataReader.GetInt64(2);
                                instance.<ClientIpAddress>k__BackingField = dataReader.IsDBNull(3) ? default(string) : dataReader.GetString(3);
                                instance.<ConnectedAt>k__BackingField = dataReader.GetDateTime(4);
                                instance.<DisconnectedAt>k__BackingField = dataReader.IsDBNull(5) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(5);
                                instance.<LastAccuracy>k__BackingField = dataReader.IsDBNull(6) ? default(double?) : (double?)dataReader.GetDouble(6);
                                instance.<LastLatitude>k__BackingField = dataReader.IsDBNull(7) ? default(double?) : (double?)dataReader.GetDouble(7);
                                instance.<LastLongitude>k__BackingField = dataReader.IsDBNull(8) ? default(double?) : (double?)dataReader.GetDouble(8);
                                instance.<LastPositionAt>k__BackingField = dataReader.IsDBNull(9) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(9);
                                instance.<LastStreamPauseAt>k__BackingField = dataReader.IsDBNull(10) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(10);
                                instance.<MountPointId>k__BackingField = dataReader.GetInt32(11);
                                instance.<SerialNumber>k__BackingField = dataReader.GetInt32(12);
                                instance.<Status>k__BackingField = Invoke(((EnumToNumberConverter<ClientStreamStatus, int>)((IReadOnlyProperty)[LIFTABLE Constant: Property: ClientSession.Status (ClientStreamStatus) Required | Resolver: namelessParameter{2} => namelessParameter{2}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.ClientSession").FindProperty("Status")]).GetTypeMapping().Converter).ConvertFromProviderTyped, dataReader.GetInt32(13));
                                instance.<UserId>k__BackingField = dataReader.GetString(14);
                                (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                    context: materializationContext1.Context,
                                    entity: instance,
                                    queryTrackingBehavior: NoTracking,
                                    structuralType: [LIFTABLE Constant: EntityType: ClientSession | Resolver: namelessParameter{3} => namelessParameter{3}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.ClientSession")]) : default(void);
                                return instance;
                            }}
                    default:
                        default(ClientSession)
                }
                ;
                return instance1;
            } :
            {
                object[] keyValues1;
                keyValues1 = new object[]{ (object)dataReader.GetString(0) };
                return ShapedQueryCompilingExpressionVisitor.CreateNullKeyValueInNoTrackingQuery(
                    entityType: [LIFTABLE Constant: EntityType: ClientSession | Resolver: namelessParameter{4} => namelessParameter{4}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.ClientSession")],
                    properties: [LIFTABLE Constant: List<RuntimeProperty> { Property: ClientSession.Id (string) Required PK AfterSave:Throw } | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.ClientSession").FindPrimaryKey().Properties],
                    keyValues: keyValues1);
            };
            return instance1;
        };
        return entity;
    },
    contextType: AgOpenNtripCaster.Server.Data.ApplicationDbContext,
    standAloneStateManager: False,
    detailedErrorsEnabled: False,
    threadSafetyChecksEnabled: True)'
[23:10:02 DBG] Creating DbConnection.
[23:10:02 DBG] Created DbConnection. (1ms).
[23:10:02 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:02 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:02 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:02 DBG] Created DbCommand for 'ExecuteReader' (2ms).
[23:10:02 DBG] Initialized DbCommand for 'ExecuteReader' (4ms).
[23:10:02 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:02 INF] Executed DbCommand (10ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:02 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:02 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 9ms reading results.
[23:10:02 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:02 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (10ms).
[23:10:02 DBG] Compiling query expression:
'DbSet<SourceConnection>()
    .AsNoTracking()
    .Where(sc => sc.DisconnectedAt == null)'
[23:10:02 DBG] Generated query execution expression:
'queryContext => SingleQueryingEnumerable.Create<SourceConnection>(
    relationalQueryContext: (RelationalQueryContext)queryContext,
    relationalCommandResolver: parameters => [LIFTABLE Constant: RelationalCommandCache.QueryExpression(
        Projection Mapping:
            EmptyProjectionMember -> Dictionary<IProperty, int> { [Property: SourceConnection.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 0], [Property: SourceConnection.BytesReceived (long) Required, 1], [Property: SourceConnection.BytesSent (long) Required, 2], [Property: SourceConnection.ConnectedAt (DateTime) Required, 3], [Property: SourceConnection.DisconnectedAt (DateTime?), 4], [Property: SourceConnection.MountPointId (int) Required FK Index, 5], [Property: SourceConnection.Status (SourceConnectionStatus) Required, 6] }
        SELECT s.Id, s.BytesReceived, s.BytesSent, s.ConnectedAt, s.DisconnectedAt, s.MountPointId, s.Status
        FROM SourceConnections AS s
        WHERE s.DisconnectedAt == NULL) | Resolver: c => new RelationalCommandCache(
        c.Dependencies.MemoryCache,
        c.RelationalDependencies.QuerySqlGeneratorFactory,
        c.RelationalDependencies.RelationalParameterBasedSqlProcessorFactory,
        Projection Mapping:
            EmptyProjectionMember -> Dictionary<IProperty, int> { [Property: SourceConnection.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 0], [Property: SourceConnection.BytesReceived (long) Required, 1], [Property: SourceConnection.BytesSent (long) Required, 2], [Property: SourceConnection.ConnectedAt (DateTime) Required, 3], [Property: SourceConnection.DisconnectedAt (DateTime?), 4], [Property: SourceConnection.MountPointId (int) Required FK Index, 5], [Property: SourceConnection.Status (SourceConnectionStatus) Required, 6] }
        SELECT s.Id, s.BytesReceived, s.BytesSent, s.ConnectedAt, s.DisconnectedAt, s.MountPointId, s.Status
        FROM SourceConnections AS s
        WHERE s.DisconnectedAt == NULL,
        False,
        new HashSet<string>(
            new string[]{ },
            StringComparer.Ordinal
        )
    )].GetRelationalCommandTemplate(parameters),
    readerColumns: null,
    shaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
    {
        SourceConnection entity;
        entity =
        {
            MaterializationContext materializationContext1;
            IEntityType entityType1;
            SourceConnection instance1;
            materializationContext1 = new MaterializationContext(
                [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                queryContext.Context
            );
            instance1 = default(SourceConnection);
            (object)dataReader.GetInt32(0) != null ?
            {
                ISnapshot shadowSnapshot1;
                shadowSnapshot1 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                entityType1 = [LIFTABLE Constant: EntityType: SourceConnection | Resolver: namelessParameter{0} => namelessParameter{0}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection")];
                instance1 = switch (entityType1)
                {
                    case [LIFTABLE Constant: EntityType: SourceConnection | Resolver: namelessParameter{1} => namelessParameter{1}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection")]:
                        {
                            return
                            {
                                SourceConnection instance;
                                instance = new SourceConnection();
                                instance.<Id>k__BackingField = dataReader.GetInt32(0);
                                instance.<BytesReceived>k__BackingField = dataReader.GetInt64(1);
                                instance.<BytesSent>k__BackingField = dataReader.GetInt64(2);
                                instance.<ConnectedAt>k__BackingField = dataReader.GetDateTime(3);
                                instance.<DisconnectedAt>k__BackingField = dataReader.IsDBNull(4) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(4);
                                instance.<MountPointId>k__BackingField = dataReader.GetInt32(5);
                                instance.<Status>k__BackingField = Invoke(((EnumToNumberConverter<SourceConnectionStatus, int>)((IReadOnlyProperty)[LIFTABLE Constant: Property: SourceConnection.Status (SourceConnectionStatus) Required | Resolver: namelessParameter{2} => namelessParameter{2}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection").FindProperty("Status")]).GetTypeMapping().Converter).ConvertFromProviderTyped, dataReader.GetInt32(6));
                                (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                    context: materializationContext1.Context,
                                    entity: instance,
                                    queryTrackingBehavior: NoTracking,
                                    structuralType: [LIFTABLE Constant: EntityType: SourceConnection | Resolver: namelessParameter{3} => namelessParameter{3}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection")]) : default(void);
                                return instance;
                            }}
                    default:
                        default(SourceConnection)
                }
                ;
                return instance1;
            } :
            {
                object[] keyValues1;
                keyValues1 = new object[]{ (object)dataReader.GetInt32(0) };
                return ShapedQueryCompilingExpressionVisitor.CreateNullKeyValueInNoTrackingQuery(
                    entityType: [LIFTABLE Constant: EntityType: SourceConnection | Resolver: namelessParameter{4} => namelessParameter{4}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection")],
                    properties: [LIFTABLE Constant: List<RuntimeProperty> { Property: SourceConnection.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd } | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection").FindPrimaryKey().Properties],
                    keyValues: keyValues1);
            };
            return instance1;
        };
        return entity;
    },
    contextType: AgOpenNtripCaster.Server.Data.ApplicationDbContext,
    standAloneStateManager: False,
    detailedErrorsEnabled: False,
    threadSafetyChecksEnabled: True)'
[23:10:02 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:02 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:02 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:02 DBG] Created DbCommand for 'ExecuteReader' (11ms).
[23:10:02 DBG] Initialized DbCommand for 'ExecuteReader' (22ms).
[23:10:02 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:10:02 INF] Executed DbCommand (17ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:10:03 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:03 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 7ms reading results.
[23:10:03 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:03 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (5ms).
[23:10:03 DBG] 'ApplicationDbContext' disposed.
[23:10:03 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:03 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (4ms).
[23:10:12 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:12 DBG] Creating DbConnection.
[23:10:12 DBG] Created DbConnection. (5ms).
[23:10:12 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:12 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:12 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:12 DBG] Created DbCommand for 'ExecuteReader' (6ms).
[23:10:12 DBG] Initialized DbCommand for 'ExecuteReader' (16ms).
[23:10:12 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:12 INF] Executed DbCommand (17ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:12 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:12 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 12ms reading results.
[23:10:12 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:12 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (16ms).
[23:10:12 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:12 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:12 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:12 DBG] Created DbCommand for 'ExecuteReader' (7ms).
[23:10:12 DBG] Initialized DbCommand for 'ExecuteReader' (15ms).
[23:10:12 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:10:12 INF] Executed DbCommand (6ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:10:12 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:12 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 1ms reading results.
[23:10:12 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:12 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (1ms).
[23:10:12 DBG] 'ApplicationDbContext' disposed.
[23:10:12 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:12 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (1ms).
[23:10:22 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:22 DBG] Creating DbConnection.
[23:10:22 DBG] Created DbConnection. (46ms).
[23:10:22 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:22 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:22 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:22 DBG] Created DbCommand for 'ExecuteReader' (4ms).
[23:10:22 DBG] Initialized DbCommand for 'ExecuteReader' (12ms).
[23:10:22 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:22 INF] Executed DbCommand (19ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:22 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:22 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 7ms reading results.
[23:10:22 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:22 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (3ms).
[23:10:22 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:22 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:22 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:22 DBG] Created DbCommand for 'ExecuteReader' (6ms).
[23:10:22 DBG] Initialized DbCommand for 'ExecuteReader' (14ms).
[23:10:22 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:10:22 INF] Executed DbCommand (11ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:10:22 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:22 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 3ms reading results.
[23:10:22 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:22 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (5ms).
[23:10:22 DBG] 'ApplicationDbContext' disposed.
[23:10:22 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:22 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (7ms).
[23:10:32 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:32 DBG] Creating DbConnection.
[23:10:32 DBG] Created DbConnection. (2ms).
[23:10:32 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:32 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:32 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:32 DBG] Created DbCommand for 'ExecuteReader' (1ms).
[23:10:32 DBG] Initialized DbCommand for 'ExecuteReader' (3ms).
[23:10:32 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:32 INF] Executed DbCommand (20ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:32 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:32 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 2ms reading results.
[23:10:32 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:32 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (5ms).
[23:10:32 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:32 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:32 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:32 DBG] Created DbCommand for 'ExecuteReader' (2ms).
[23:10:32 DBG] Initialized DbCommand for 'ExecuteReader' (5ms).
[23:10:32 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:10:32 INF] Executed DbCommand (16ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:10:32 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:32 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 5ms reading results.
[23:10:32 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:32 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (1ms).
[23:10:32 DBG] 'ApplicationDbContext' disposed.
[23:10:32 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:32 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (4ms).
[23:10:37 DBG] Connection id "0HNGR4N8NSOJ1" accepted.
[23:10:37 DBG] Connection id "0HNGR4N8NSOJ2" accepted.
[23:10:37 DBG] Connection id "0HNGR4N8NSOJ1" started.
[23:10:37 DBG] Connection id "0HNGR4N8NSOJ2" started.
[23:10:37 INF] Request starting HTTP/1.1 OPTIONS http://localhost:5000/api/users/me - null null
[23:10:37 INF] Request starting HTTP/1.1 OPTIONS http://localhost:5000/api/users/me - null null
[23:10:37 DBG] 1 candidate(s) found for the request path '/api/users/me'
[23:10:37 DBG] 1 candidate(s) found for the request path '/api/users/me'
[23:10:37 DBG] Request matched endpoint '405 HTTP Method Not Supported'
[23:10:37 DBG] Request matched endpoint '405 HTTP Method Not Supported'
[23:10:37 WRN] Failed to determine the https port for redirect.
[23:10:37 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:37 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:37 INF] CORS policy execution successful.
[23:10:37 INF] CORS policy execution successful.
[23:10:37 DBG] The request is a preflight request.
[23:10:37 DBG] The request is a preflight request.
[23:10:37 INF] HTTP OPTIONS /api/users/me responded 204 in 49.9130 ms
[23:10:37 INF] HTTP OPTIONS /api/users/me responded 204 in 48.5348 ms
[23:10:37 DBG] Connection id "0HNGR4N8NSOJ1" completed keep alive response.
[23:10:37 DBG] Connection id "0HNGR4N8NSOJ2" completed keep alive response.
[23:10:37 INF] Request finished HTTP/1.1 OPTIONS http://localhost:5000/api/users/me - 204 null null 309.6057ms
[23:10:37 INF] Request finished HTTP/1.1 OPTIONS http://localhost:5000/api/users/me - 204 null null 310.9494ms
[23:10:38 INF] Request starting HTTP/1.1 GET http://localhost:5000/api/users/me - null null
[23:10:38 DBG] 2 candidate(s) found for the request path '/api/users/me'
[23:10:38 DBG] Endpoint 'AgOpenNtripCaster.Server.Controllers.UsersController.GetCurrentUser (AgOpenNtripCaster.Server)' with route pattern 'api/Users/me' is valid for the request path '/api/users/me'
[23:10:38 DBG] Endpoint 'AgOpenNtripCaster.Server.Controllers.UsersController.GetUserById (AgOpenNtripCaster.Server)' with route pattern 'api/Users/{userId}' is valid for the request path '/api/users/me'
[23:10:38 DBG] Request matched endpoint 'AgOpenNtripCaster.Server.Controllers.UsersController.GetCurrentUser (AgOpenNtripCaster.Server)'
[23:10:38 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:38 INF] CORS policy execution successful.
[23:10:38 DBG] Successfully validated the token.
[23:10:38 DBG] AuthenticationScheme: Bearer was successfully authenticated.
[23:10:38 DBG] Authorization was successful.
[23:10:38 INF] Executing endpoint 'AgOpenNtripCaster.Server.Controllers.UsersController.GetCurrentUser (AgOpenNtripCaster.Server)'
[23:10:38 INF] Route matched with {action = "GetCurrentUser", controller = "Users"}. Executing controller action with signature System.Threading.Tasks.Task`1[Microsoft.AspNetCore.Mvc.ActionResult`1[AgOpenNtripCaster.Server.Models.DTOs.UserDto]] GetCurrentUser() on controller AgOpenNtripCaster.Server.Controllers.UsersController (AgOpenNtripCaster.Server).
[23:10:38 DBG] Execution plan of authorization filters (in the following order): ["None"]
[23:10:38 DBG] Execution plan of resource filters (in the following order): ["None"]
[23:10:38 DBG] Execution plan of action filters (in the following order): ["Microsoft.AspNetCore.Mvc.ModelBinding.UnsupportedContentTypeFilter (Order: -3000)", "Microsoft.AspNetCore.Mvc.Infrastructure.ModelStateInvalidFilter (Order: -2000)"]
[23:10:38 DBG] Execution plan of exception filters (in the following order): ["None"]
[23:10:38 DBG] Execution plan of result filters (in the following order): ["Microsoft.AspNetCore.Mvc.Infrastructure.ClientErrorResultFilter (Order: -2000)"]
[23:10:38 DBG] Executing controller factory for controller AgOpenNtripCaster.Server.Controllers.UsersController (AgOpenNtripCaster.Server)
[23:10:38 DBG] Executed controller factory for controller AgOpenNtripCaster.Server.Controllers.UsersController (AgOpenNtripCaster.Server)
[23:10:38 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:38 DBG] Compiling query expression:
'DbSet<NtripUser>()
    .Include(u => u.Groups)
    .FirstOrDefault(u => u.Id == __userId_0)'
[23:10:38 DBG] Including navigation: 'NtripUser.Groups'.
[23:10:38 DBG] Generated query execution expression:
'queryContext => ShapedQueryCompilingExpressionVisitor.SingleOrDefaultAsync<NtripUser>(
    asyncEnumerable: SingleQueryingEnumerable.Create<NtripUser>(
        relationalQueryContext: (RelationalQueryContext)queryContext,
        relationalCommandResolver: parameters => [LIFTABLE Constant: RelationalCommandCache.QueryExpression(
            Client Projections:
                0 -> Dictionary<IProperty, int> { [Property: NtripUser.Id (string) Required PK AfterSave:Throw, 0], [Property: NtripUser.AccessFailedCount (int) Required, 1], [Property: NtripUser.ConcurrencyStamp (string) Concurrency, 2], [Property: NtripUser.CreatedAt (DateTime) Required, 3], [Property: NtripUser.Email (string) MaxLength(256), 4], [Property: NtripUser.EmailConfirmed (bool) Required, 5], [Property: NtripUser.FullName (string) Required, 6], [Property: NtripUser.IsActive (bool) Required, 7], [Property: NtripUser.LastGeneratedSourcePassword (string), 8], [Property: NtripUser.LockoutEnabled (bool) Required, 9], [Property: NtripUser.LockoutEnd (DateTimeOffset?), 10], [Property: NtripUser.MaxConnections (int) Required, 11], [Property: NtripUser.NormalizedEmail (string) Index MaxLength(256), 12], [Property: NtripUser.NormalizedUserName (string) Index MaxLength(256), 13], [Property: NtripUser.PasswordHash (string), 14], [Property: NtripUser.PhoneNumber (string), 15], [Property: NtripUser.PhoneNumberConfirmed (bool) Required, 16], [Property: NtripUser.RefreshToken (string), 17], [Property: NtripUser.RefreshTokenExpires (DateTime?), 18], [Property: NtripUser.SecurityStamp (string), 19], [Property: NtripUser.SourcePassword (string), 20], [Property: NtripUser.SourcePasswordGeneratedAt (DateTime?), 21], [Property: NtripUser.TwoFactorEnabled (bool) Required, 22], [Property: NtripUser.UserName (string) MaxLength(256), 23] }
                1 -> 0
                2 -> Dictionary<IProperty, int> { [Property: NtripGroupNtripUser (Dictionary<string, object>).GroupsId (no field, int) Indexer Required PK FK AfterSave:Throw, 24], [Property: NtripGroupNtripUser (Dictionary<string, object>).UsersId (no field, string) Indexer Required PK FK Index AfterSave:Throw, 25] }
                3 -> Dictionary<IProperty, int> { [Property: NtripGroup.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 26], [Property: NtripGroup.CreatedAt (DateTime) Required, 27], [Property: NtripGroup.Description (string) Required, 28], [Property: NtripGroup.IsActive (bool) Required, 29], [Property: NtripGroup.Name (string) Required, 30] }
                4 -> 24
                5 -> 25
                6 -> 26
            SELECT a0.Id, a0.AccessFailedCount, a0.ConcurrencyStamp, a0.CreatedAt, a0.Email, a0.EmailConfirmed, a0.FullName, a0.IsActive, a0.LastGeneratedSourcePassword, a0.LockoutEnabled, a0.LockoutEnd, a0.MaxConnections, a0.NormalizedEmail, a0.NormalizedUserName, a0.PasswordHash, a0.PhoneNumber, a0.PhoneNumberConfirmed, a0.RefreshToken, a0.RefreshTokenExpires, a0.SecurityStamp, a0.SourcePassword, a0.SourcePasswordGeneratedAt, a0.TwoFactorEnabled, a0.UserName, s.GroupsId, s.UsersId, s.Id, s.CreatedAt, s.Description, s.IsActive, s.Name
            FROM
            (
                SELECT TOP(1) a.Id, a.AccessFailedCount, a.ConcurrencyStamp, a.CreatedAt, a.Email, a.EmailConfirmed, a.FullName, a.IsActive, a.LastGeneratedSourcePassword, a.LockoutEnabled, a.LockoutEnd, a.MaxConnections, a.NormalizedEmail, a.NormalizedUserName, a.PasswordHash, a.PhoneNumber, a.PhoneNumberConfirmed, a.RefreshToken, a.RefreshTokenExpires, a.SecurityStamp, a.SourcePassword, a.SourcePasswordGeneratedAt, a.TwoFactorEnabled, a.UserName
                FROM AspNetUsers AS a
                WHERE a.Id == @__userId_0
            ) AS a0
            LEFT JOIN
            (
                SELECT u.GroupsId, u.UsersId, n.Id, n.CreatedAt, n.Description, n.IsActive, n.Name
                FROM UserGroups AS u
                INNER JOIN NtripGroups AS n ON u.GroupsId == n.Id
            ) AS s ON a0.Id == s.UsersId
            ORDER BY a0.Id ASC, s.GroupsId ASC, s.UsersId ASC) | Resolver: c => new RelationalCommandCache(
            c.Dependencies.MemoryCache,
            c.RelationalDependencies.QuerySqlGeneratorFactory,
            c.RelationalDependencies.RelationalParameterBasedSqlProcessorFactory,
            Client Projections:
                0 -> Dictionary<IProperty, int> { [Property: NtripUser.Id (string) Required PK AfterSave:Throw, 0], [Property: NtripUser.AccessFailedCount (int) Required, 1], [Property: NtripUser.ConcurrencyStamp (string) Concurrency, 2], [Property: NtripUser.CreatedAt (DateTime) Required, 3], [Property: NtripUser.Email (string) MaxLength(256), 4], [Property: NtripUser.EmailConfirmed (bool) Required, 5], [Property: NtripUser.FullName (string) Required, 6], [Property: NtripUser.IsActive (bool) Required, 7], [Property: NtripUser.LastGeneratedSourcePassword (string), 8], [Property: NtripUser.LockoutEnabled (bool) Required, 9], [Property: NtripUser.LockoutEnd (DateTimeOffset?), 10], [Property: NtripUser.MaxConnections (int) Required, 11], [Property: NtripUser.NormalizedEmail (string) Index MaxLength(256), 12], [Property: NtripUser.NormalizedUserName (string) Index MaxLength(256), 13], [Property: NtripUser.PasswordHash (string), 14], [Property: NtripUser.PhoneNumber (string), 15], [Property: NtripUser.PhoneNumberConfirmed (bool) Required, 16], [Property: NtripUser.RefreshToken (string), 17], [Property: NtripUser.RefreshTokenExpires (DateTime?), 18], [Property: NtripUser.SecurityStamp (string), 19], [Property: NtripUser.SourcePassword (string), 20], [Property: NtripUser.SourcePasswordGeneratedAt (DateTime?), 21], [Property: NtripUser.TwoFactorEnabled (bool) Required, 22], [Property: NtripUser.UserName (string) MaxLength(256), 23] }
                1 -> 0
                2 -> Dictionary<IProperty, int> { [Property: NtripGroupNtripUser (Dictionary<string, object>).GroupsId (no field, int) Indexer Required PK FK AfterSave:Throw, 24], [Property: NtripGroupNtripUser (Dictionary<string, object>).UsersId (no field, string) Indexer Required PK FK Index AfterSave:Throw, 25] }
                3 -> Dictionary<IProperty, int> { [Property: NtripGroup.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 26], [Property: NtripGroup.CreatedAt (DateTime) Required, 27], [Property: NtripGroup.Description (string) Required, 28], [Property: NtripGroup.IsActive (bool) Required, 29], [Property: NtripGroup.Name (string) Required, 30] }
                4 -> 24
                5 -> 25
                6 -> 26
            SELECT a0.Id, a0.AccessFailedCount, a0.ConcurrencyStamp, a0.CreatedAt, a0.Email, a0.EmailConfirmed, a0.FullName, a0.IsActive, a0.LastGeneratedSourcePassword, a0.LockoutEnabled, a0.LockoutEnd, a0.MaxConnections, a0.NormalizedEmail, a0.NormalizedUserName, a0.PasswordHash, a0.PhoneNumber, a0.PhoneNumberConfirmed, a0.RefreshToken, a0.RefreshTokenExpires, a0.SecurityStamp, a0.SourcePassword, a0.SourcePasswordGeneratedAt, a0.TwoFactorEnabled, a0.UserName, s.GroupsId, s.UsersId, s.Id, s.CreatedAt, s.Description, s.IsActive, s.Name
            FROM
            (
                SELECT TOP(1) a.Id, a.AccessFailedCount, a.ConcurrencyStamp, a.CreatedAt, a.Email, a.EmailConfirmed, a.FullName, a.IsActive, a.LastGeneratedSourcePassword, a.LockoutEnabled, a.LockoutEnd, a.MaxConnections, a.NormalizedEmail, a.NormalizedUserName, a.PasswordHash, a.PhoneNumber, a.PhoneNumberConfirmed, a.RefreshToken, a.RefreshTokenExpires, a.SecurityStamp, a.SourcePassword, a.SourcePasswordGeneratedAt, a.TwoFactorEnabled, a.UserName
                FROM AspNetUsers AS a
                WHERE a.Id == @__userId_0
            ) AS a0
            LEFT JOIN
            (
                SELECT u.GroupsId, u.UsersId, n.Id, n.CreatedAt, n.Description, n.IsActive, n.Name
                FROM UserGroups AS u
                INNER JOIN NtripGroups AS n ON u.GroupsId == n.Id
            ) AS s ON a0.Id == s.UsersId
            ORDER BY a0.Id ASC, s.GroupsId ASC, s.UsersId ASC,
            False,
            new HashSet<string>(
                new string[]{ },
                StringComparer.Ordinal
            )
        )].GetRelationalCommandTemplate(parameters),
        readerColumns: null,
        shaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
        {
            resultContext.Values == null ?
            {
                NtripUser entity;
                entity =
                {
                    MaterializationContext materializationContext1;
                    IEntityType entityType1;
                    NtripUser instance1;
                    InternalEntityEntry entry1;
                    bool hasNullKey1;
                    materializationContext1 = new MaterializationContext(
                        [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                        queryContext.Context
                    );
                    instance1 = default(NtripUser);
                    entry1 = queryContext.TryGetEntry(
                        key: [LIFTABLE Constant: Key: NtripUser.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser").FindPrimaryKey()],
                        keyValues: new object[]{ (object)dataReader.GetString(0) },
                        throwOnNullKey: True,
                        hasNullKey: hasNullKey1);
                    !(hasNullKey1) ? entry1 != default(InternalEntityEntry) ?
                    {
                        entityType1 = entry1.EntityType;
                        return instance1 = (NtripUser)entry1.Entity;
                    } :
                    {
                        ISnapshot shadowSnapshot1;
                        shadowSnapshot1 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                        entityType1 = [LIFTABLE Constant: EntityType: NtripUser | Resolver: namelessParameter{0} => namelessParameter{0}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser")];
                        instance1 = switch (entityType1)
                        {
                            case [LIFTABLE Constant: EntityType: NtripUser | Resolver: namelessParameter{1} => namelessParameter{1}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser")]:
                                {
                                    return
                                    {
                                        NtripUser instance;
                                        instance = new NtripUser();
                                        instance.<Id>k__BackingField = dataReader.GetString(0);
                                        instance.<AccessFailedCount>k__BackingField = dataReader.GetInt32(1);
                                        instance.<ConcurrencyStamp>k__BackingField = dataReader.IsDBNull(2) ? default(string) : dataReader.GetString(2);
                                        instance.<CreatedAt>k__BackingField = dataReader.GetDateTime(3);
                                        instance.<Email>k__BackingField = dataReader.IsDBNull(4) ? default(string) : dataReader.GetString(4);
                                        instance.<EmailConfirmed>k__BackingField = dataReader.GetBoolean(5);
                                        instance.<FullName>k__BackingField = dataReader.GetString(6);
                                        instance.<IsActive>k__BackingField = dataReader.GetBoolean(7);
                                        instance.<LastGeneratedSourcePassword>k__BackingField = dataReader.IsDBNull(8) ? default(string) : dataReader.GetString(8);
                                        instance.<LockoutEnabled>k__BackingField = dataReader.GetBoolean(9);
                                        instance.<LockoutEnd>k__BackingField = dataReader.IsDBNull(10) ? default(DateTimeOffset?) : (DateTimeOffset?)dataReader.GetFieldValue<DateTimeOffset>(10);
                                        instance.<MaxConnections>k__BackingField = dataReader.GetInt32(11);
                                        instance.<NormalizedEmail>k__BackingField = dataReader.IsDBNull(12) ? default(string) : dataReader.GetString(12);
                                        instance.<NormalizedUserName>k__BackingField = dataReader.IsDBNull(13) ? default(string) : dataReader.GetString(13);
                                        instance.<PasswordHash>k__BackingField = dataReader.IsDBNull(14) ? default(string) : dataReader.GetString(14);
                                        instance.<PhoneNumber>k__BackingField = dataReader.IsDBNull(15) ? default(string) : dataReader.GetString(15);
                                        instance.<PhoneNumberConfirmed>k__BackingField = dataReader.GetBoolean(16);
                                        instance.<RefreshToken>k__BackingField = dataReader.IsDBNull(17) ? default(string) : dataReader.GetString(17);
                                        instance.<RefreshTokenExpires>k__BackingField = dataReader.IsDBNull(18) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(18);
                                        instance.<SecurityStamp>k__BackingField = dataReader.IsDBNull(19) ? default(string) : dataReader.GetString(19);
                                        instance.<SourcePassword>k__BackingField = dataReader.IsDBNull(20) ? default(string) : dataReader.GetString(20);
                                        instance.<SourcePasswordGeneratedAt>k__BackingField = dataReader.IsDBNull(21) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(21);
                                        instance.<TwoFactorEnabled>k__BackingField = dataReader.GetBoolean(22);
                                        instance.<UserName>k__BackingField = dataReader.IsDBNull(23) ? default(string) : dataReader.GetString(23);
                                        (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                            context: materializationContext1.Context,
                                            entity: instance,
                                            queryTrackingBehavior: TrackAll,
                                            structuralType: [LIFTABLE Constant: EntityType: NtripUser | Resolver: namelessParameter{2} => namelessParameter{2}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser")]) : default(void);
                                        return instance;
                                    }}
                            default:
                                default(NtripUser)
                        }
                        ;
                        entry1 = entityType1 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                            entityType: entityType1,
                            entity: instance1,
                            snapshot: shadowSnapshot1);
                        return instance1;
                    } : default(void);
                    return instance1;
                };
                resultContext.Values = new object[]{ entity };
                ShaperProcessingExpressionVisitor.InitializeIncludeCollection<NtripUser, NtripUser>(
                    collectionId: 0,
                    queryContext: queryContext,
                    dbDataReader: dataReader,
                    resultCoordinator: resultCoordinator,
                    entity: (NtripUser)(resultContext.Values[0]),
                    parentIdentifier: [LIFTABLE Constant: Func<QueryContext, DbDataReader, object[]> | Resolver: _ => (queryContext, dataReader) => new object[]{ dataReader.GetString(0) }],
                    outerIdentifier: [LIFTABLE Constant: Func<QueryContext, DbDataReader, object[]> | Resolver: _ => (queryContext, dataReader) => new object[]{ dataReader.GetString(0) }],
                    navigation: [LIFTABLE Constant: SkipNavigation: NtripUser.Groups (ICollection<NtripGroup>) CollectionNtripGroup Inverse: Users | Resolver: namelessParameter{3} => namelessParameter{3}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser").FindSkipNavigation("Groups")],
                    clrCollectionAccessor: [LIFTABLE Constant: ClrICollectionAccessor<NtripUser, ICollection<NtripGroup>, NtripGroup> | Resolver: namelessParameter{4} => namelessParameter{4}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser").FindSkipNavigation("Groups").GetCollectionAccessor()],
                    trackingQuery: True,
                    setLoaded: True);
            } : default(void);
            ShaperProcessingExpressionVisitor.PopulateIncludeCollection<NtripUser, NtripGroup>(
                collectionId: 0,
                queryContext: queryContext,
                dbDataReader: dataReader,
                resultCoordinator: resultCoordinator,
                parentIdentifier: [LIFTABLE Constant: Func<QueryContext, DbDataReader, object[]> | Resolver: _ => (queryContext, dataReader) => new object[]{ dataReader.GetString(0) }],
                outerIdentifier: [LIFTABLE Constant: Func<QueryContext, DbDataReader, object[]> | Resolver: _ => (queryContext, dataReader) => new object[]{ dataReader.GetString(0) }],
                selfIdentifier: [LIFTABLE Constant: Func<QueryContext, DbDataReader, object[]> | Resolver: _ => (queryContext, dataReader) => new object[]
                {
                    (object)dataReader.IsDBNull(24) ? default(int?) : (int?)dataReader.GetInt32(24),
                    dataReader.IsDBNull(25) ? default(string) : dataReader.GetString(25),
                    (object)dataReader.IsDBNull(26) ? default(int?) : (int?)dataReader.GetInt32(26)
                }],
                parentIdentifierValueComparers: [LIFTABLE Constant: Func<object, object, bool>[] { Func<object, object, bool> } | Resolver: _ => new Func<object, object, bool>[]{ (left, right) => left == null ? right == null : right != null && (string)left == (string)right }],
                outerIdentifierValueComparers: [LIFTABLE Constant: Func<object, object, bool>[] { Func<object, object, bool> } | Resolver: _ => new Func<object, object, bool>[]{ (left, right) => left == null ? right == null : right != null && (string)left == (string)right }],
                selfIdentifierValueComparers: [LIFTABLE Constant: Func<object, object, bool>[] { Func<object, object, bool>, Func<object, object, bool>, Func<object, object, bool> } | Resolver: _ => new Func<object, object, bool>[]
                {
                    (left, right) => left == null ? right == null : right != null && (int)left == (int)right,
                    (left, right) => left == null ? right == null : right != null && (string)left == (string)right,
                    (left, right) => left == null ? right == null : right != null && (int)left == (int)right
                }],
                innerShaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
                {
                    Dictionary<string, object> entity;
                    NtripGroup entity;
                    entity =
                    {
                        MaterializationContext materializationContext2;
                        IEntityType entityType2;
                        Dictionary<string, object> instance2;
                        InternalEntityEntry entry2;
                        bool hasNullKey2;
                        materializationContext2 = new MaterializationContext(
                            [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                            queryContext.Context
                        );
                        instance2 = default(Dictionary<string, object>);
                        entry2 = queryContext.TryGetEntry(
                            key: [LIFTABLE Constant: Key: NtripGroupNtripUser (Dictionary<string, object>).GroupsId, NtripGroupNtripUser (Dictionary<string, object>).UsersId PK | Resolver: c => c.Dependencies.Model.FindEntityType("NtripGroupNtripUser").FindPrimaryKey()],
                            keyValues: new object[]
                            {
                                dataReader.IsDBNull(24) ? default(object) : (object)dataReader.GetInt32(24),
                                dataReader.IsDBNull(25) ? default(object) : (object)dataReader.GetString(25)
                            },
                            throwOnNullKey: False,
                            hasNullKey: hasNullKey2);
                        !(hasNullKey2) ? entry2 != default(InternalEntityEntry) ?
                        {
                            entityType2 = entry2.EntityType;
                            return instance2 = (Dictionary<string, object>)entry2.Entity;
                        } :
                        {
                            ISnapshot shadowSnapshot2;
                            shadowSnapshot2 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                            entityType2 = [LIFTABLE Constant: EntityType: NtripGroupNtripUser (Dictionary<string, object>) CLR Type: Dictionary<string, object> | Resolver: namelessParameter{5} => namelessParameter{5}.Dependencies.Model.FindEntityType("NtripGroupNtripUser")];
                            instance2 = switch (entityType2)
                            {
                                case [LIFTABLE Constant: EntityType: NtripGroupNtripUser (Dictionary<string, object>) CLR Type: Dictionary<string, object> | Resolver: namelessParameter{6} => namelessParameter{6}.Dependencies.Model.FindEntityType("NtripGroupNtripUser")]:
                                    {
                                        return
                                        {
                                            Dictionary<string, object> instance;
                                            instance = new Dictionary<string, object>();
                                            instance["GroupsId"] = dataReader.IsDBNull(24) ? default(object) : (object)dataReader.GetInt32(24);
                                            instance["UsersId"] = dataReader.IsDBNull(25) ? default(object) : (object)dataReader.GetString(25);
                                            (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                                context: materializationContext2.Context,
                                                entity: instance,
                                                queryTrackingBehavior: TrackAll,
                                                structuralType: [LIFTABLE Constant: EntityType: NtripGroupNtripUser (Dictionary<string, object>) CLR Type: Dictionary<string, object> | Resolver: namelessParameter{7} => namelessParameter{7}.Dependencies.Model.FindEntityType("NtripGroupNtripUser")]) : default(void);
                                            return instance;
                                        }}
                                default:
                                    default(Dictionary<string, object>)
                            }
                            ;
                            entry2 = entityType2 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                                entityType: entityType2,
                                entity: instance2,
                                snapshot: shadowSnapshot2);
                            return instance2;
                        } : default(void);
                        return instance2;
                    };
                    entity =
                    {
                        MaterializationContext materializationContext3;
                        IEntityType entityType3;
                        NtripGroup instance3;
                        InternalEntityEntry entry3;
                        bool hasNullKey3;
                        materializationContext3 = new MaterializationContext(
                            [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                            queryContext.Context
                        );
                        instance3 = default(NtripGroup);
                        entry3 = queryContext.TryGetEntry(
                            key: [LIFTABLE Constant: Key: NtripGroup.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup").FindPrimaryKey()],
                            keyValues: new object[]{ dataReader.IsDBNull(26) ? default(object) : (object)dataReader.GetInt32(26) },
                            throwOnNullKey: False,
                            hasNullKey: hasNullKey3);
                        !(hasNullKey3) ? entry3 != default(InternalEntityEntry) ?
                        {
                            entityType3 = entry3.EntityType;
                            return instance3 = (NtripGroup)entry3.Entity;
                        } :
                        {
                            ISnapshot shadowSnapshot3;
                            shadowSnapshot3 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                            entityType3 = [LIFTABLE Constant: EntityType: NtripGroup | Resolver: namelessParameter{8} => namelessParameter{8}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup")];
                            instance3 = switch (entityType3)
                            {
                                case [LIFTABLE Constant: EntityType: NtripGroup | Resolver: namelessParameter{9} => namelessParameter{9}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup")]:
                                    {
                                        return
                                        {
                                            NtripGroup instance;
                                            instance = new NtripGroup();
                                            instance.<Id>k__BackingField = dataReader.IsDBNull(26) ? default(int) : dataReader.GetInt32(26);
                                            instance.<CreatedAt>k__BackingField = dataReader.IsDBNull(27) ? default(DateTime) : dataReader.GetDateTime(27);
                                            instance.<Description>k__BackingField = dataReader.IsDBNull(28) ? default(string) : dataReader.GetString(28);
                                            instance.<IsActive>k__BackingField = dataReader.IsDBNull(29) ? default(bool) : dataReader.GetBoolean(29);
                                            instance.<Name>k__BackingField = dataReader.IsDBNull(30) ? default(string) : dataReader.GetString(30);
                                            (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                                context: materializationContext3.Context,
                                                entity: instance,
                                                queryTrackingBehavior: TrackAll,
                                                structuralType: [LIFTABLE Constant: EntityType: NtripGroup | Resolver: namelessParameter{10} => namelessParameter{10}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup")]) : default(void);
                                            return instance;
                                        }}
                                default:
                                    default(NtripGroup)
                            }
                            ;
                            entry3 = entityType3 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                                entityType: entityType3,
                                entity: instance3,
                                snapshot: shadowSnapshot3);
                            return instance3;
                        } : default(void);
                        return instance3;
                    };
                    return NavigationExpandingExpressionVisitor.FetchJoinEntity<Dictionary<string, object>, NtripGroup>(
                        joinEntity: entity,
                        targetEntity: entity);
                },
                inverseNavigation: [LIFTABLE Constant: SkipNavigation: NtripGroup.Users (ICollection<NtripUser>) CollectionNtripUser Inverse: Groups | Resolver: namelessParameter{11} => namelessParameter{11}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup").FindSkipNavigation("Users")],
                fixup: (namelessParameter{12}, namelessParameter{13}) =>
                {
                    [LIFTABLE Constant: ClrICollectionAccessor<NtripUser, ICollection<NtripGroup>, NtripGroup> | Resolver: namelessParameter{14} => namelessParameter{14}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser").FindSkipNavigation("Groups").GetCollectionAccessor()].Add(
                        entity: namelessParameter{12},
                        value: namelessParameter{13},
                        forMaterialization: True);
                    return [LIFTABLE Constant: ClrICollectionAccessor<NtripGroup, ICollection<NtripUser>, NtripUser> | Resolver: namelessParameter{15} => namelessParameter{15}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup").FindSkipNavigation("Users").GetCollectionAccessor()].Add(
                        entity: namelessParameter{13},
                        value: namelessParameter{12},
                        forMaterialization: True);
                },
                trackingQuery: True);
            return IsTrue(resultCoordinator.ResultReady)
             ? (NtripUser)(resultContext.Values[0]) : default(NtripUser);
        },
        contextType: AgOpenNtripCaster.Server.Data.ApplicationDbContext,
        standAloneStateManager: False,
        detailedErrorsEnabled: False,
        threadSafetyChecksEnabled: True),
    cancellationToken: queryContext.CancellationToken)'
[23:10:39 DBG] Creating DbConnection.
[23:10:39 DBG] Created DbConnection. (4ms).
[23:10:39 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:39 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:39 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:39 DBG] Created DbCommand for 'ExecuteReader' (3ms).
[23:10:39 DBG] Initialized DbCommand for 'ExecuteReader' (53ms).
[23:10:39 DBG] Executing DbCommand [Parameters=[@__userId_0='?'], CommandType='Text', CommandTimeout='30']
SELECT a0."Id", a0."AccessFailedCount", a0."ConcurrencyStamp", a0."CreatedAt", a0."Email", a0."EmailConfirmed", a0."FullName", a0."IsActive", a0."LastGeneratedSourcePassword", a0."LockoutEnabled", a0."LockoutEnd", a0."MaxConnections", a0."NormalizedEmail", a0."NormalizedUserName", a0."PasswordHash", a0."PhoneNumber", a0."PhoneNumberConfirmed", a0."RefreshToken", a0."RefreshTokenExpires", a0."SecurityStamp", a0."SourcePassword", a0."SourcePasswordGeneratedAt", a0."TwoFactorEnabled", a0."UserName", s."GroupsId", s."UsersId", s."Id", s."CreatedAt", s."Description", s."IsActive", s."Name"
FROM (
    SELECT a."Id", a."AccessFailedCount", a."ConcurrencyStamp", a."CreatedAt", a."Email", a."EmailConfirmed", a."FullName", a."IsActive", a."LastGeneratedSourcePassword", a."LockoutEnabled", a."LockoutEnd", a."MaxConnections", a."NormalizedEmail", a."NormalizedUserName", a."PasswordHash", a."PhoneNumber", a."PhoneNumberConfirmed", a."RefreshToken", a."RefreshTokenExpires", a."SecurityStamp", a."SourcePassword", a."SourcePasswordGeneratedAt", a."TwoFactorEnabled", a."UserName"
    FROM "AspNetUsers" AS a
    WHERE a."Id" = @__userId_0
    LIMIT 1
) AS a0
LEFT JOIN (
    SELECT u."GroupsId", u."UsersId", n."Id", n."CreatedAt", n."Description", n."IsActive", n."Name"
    FROM "UserGroups" AS u
    INNER JOIN "NtripGroups" AS n ON u."GroupsId" = n."Id"
) AS s ON a0."Id" = s."UsersId"
ORDER BY a0."Id", s."GroupsId", s."UsersId"
[23:10:39 INF] Executed DbCommand (41ms) [Parameters=[@__userId_0='?'], CommandType='Text', CommandTimeout='30']
SELECT a0."Id", a0."AccessFailedCount", a0."ConcurrencyStamp", a0."CreatedAt", a0."Email", a0."EmailConfirmed", a0."FullName", a0."IsActive", a0."LastGeneratedSourcePassword", a0."LockoutEnabled", a0."LockoutEnd", a0."MaxConnections", a0."NormalizedEmail", a0."NormalizedUserName", a0."PasswordHash", a0."PhoneNumber", a0."PhoneNumberConfirmed", a0."RefreshToken", a0."RefreshTokenExpires", a0."SecurityStamp", a0."SourcePassword", a0."SourcePasswordGeneratedAt", a0."TwoFactorEnabled", a0."UserName", s."GroupsId", s."UsersId", s."Id", s."CreatedAt", s."Description", s."IsActive", s."Name"
FROM (
    SELECT a."Id", a."AccessFailedCount", a."ConcurrencyStamp", a."CreatedAt", a."Email", a."EmailConfirmed", a."FullName", a."IsActive", a."LastGeneratedSourcePassword", a."LockoutEnabled", a."LockoutEnd", a."MaxConnections", a."NormalizedEmail", a."NormalizedUserName", a."PasswordHash", a."PhoneNumber", a."PhoneNumberConfirmed", a."RefreshToken", a."RefreshTokenExpires", a."SecurityStamp", a."SourcePassword", a."SourcePasswordGeneratedAt", a."TwoFactorEnabled", a."UserName"
    FROM "AspNetUsers" AS a
    WHERE a."Id" = @__userId_0
    LIMIT 1
) AS a0
LEFT JOIN (
    SELECT u."GroupsId", u."UsersId", n."Id", n."CreatedAt", n."Description", n."IsActive", n."Name"
    FROM "UserGroups" AS u
    INNER JOIN "NtripGroups" AS n ON u."GroupsId" = n."Id"
) AS s ON a0."Id" = s."UsersId"
ORDER BY a0."Id", s."GroupsId", s."UsersId"
[23:10:39 DBG] Context 'ApplicationDbContext' started tracking 'NtripUser' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:39 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:39 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 180ms reading results.
[23:10:39 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:39 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (12ms).
[23:10:39 DBG] Compiling query expression:
'DbSet<IdentityUserRole<string>>()
    .Join(
        inner: DbSet<IdentityRole>(),
        outerKeySelector: userRole => userRole.RoleId,
        innerKeySelector: role => role.Id,
        resultSelector: (userRole, role) => new {
            userRole = userRole,
            role = role
         })
    .Where(<>h__TransparentIdentifier0 => <>h__TransparentIdentifier0.userRole.UserId.Equals(__userId_0))
    .Select(<>h__TransparentIdentifier0 => <>h__TransparentIdentifier0.role.Name)'
[23:10:39 DBG] Generated query execution expression:
'queryContext => SingleQueryingEnumerable.Create<string>(
    relationalQueryContext: (RelationalQueryContext)queryContext,
    relationalCommandResolver: parameters => [LIFTABLE Constant: RelationalCommandCache.QueryExpression(
        Projection Mapping:
            EmptyProjectionMember -> 0
        SELECT a0.Name
        FROM AspNetUserRoles AS a
        INNER JOIN AspNetRoles AS a0 ON a.RoleId == a0.Id
        WHERE a.UserId == @__userId_0) | Resolver: c => new RelationalCommandCache(
        c.Dependencies.MemoryCache,
        c.RelationalDependencies.QuerySqlGeneratorFactory,
        c.RelationalDependencies.RelationalParameterBasedSqlProcessorFactory,
        Projection Mapping:
            EmptyProjectionMember -> 0
        SELECT a0.Name
        FROM AspNetUserRoles AS a
        INNER JOIN AspNetRoles AS a0 ON a.RoleId == a0.Id
        WHERE a.UserId == @__userId_0,
        False,
        new HashSet<string>(
            new string[]{ },
            StringComparer.Ordinal
        )
    )].GetRelationalCommandTemplate(parameters),
    readerColumns: null,
    shaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
    {
        string value1;
        value1 = dataReader.IsDBNull(0) ? default(string) : dataReader.GetString(0);
        return value1;
    },
    contextType: AgOpenNtripCaster.Server.Data.ApplicationDbContext,
    standAloneStateManager: False,
    detailedErrorsEnabled: False,
    threadSafetyChecksEnabled: True)'
[23:10:39 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:39 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:39 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:39 DBG] Created DbCommand for 'ExecuteReader' (3ms).
[23:10:39 DBG] Initialized DbCommand for 'ExecuteReader' (7ms).
[23:10:39 DBG] Executing DbCommand [Parameters=[@__userId_0='?'], CommandType='Text', CommandTimeout='30']
SELECT a0."Name"
FROM "AspNetUserRoles" AS a
INNER JOIN "AspNetRoles" AS a0 ON a."RoleId" = a0."Id"
WHERE a."UserId" = @__userId_0
[23:10:39 INF] Executed DbCommand (19ms) [Parameters=[@__userId_0='?'], CommandType='Text', CommandTimeout='30']
SELECT a0."Name"
FROM "AspNetUserRoles" AS a
INNER JOIN "AspNetRoles" AS a0 ON a."RoleId" = a0."Id"
WHERE a."UserId" = @__userId_0
[23:10:39 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:39 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 13ms reading results.
[23:10:39 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:39 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (1ms).
[23:10:39 DBG] List of registered output formatters, in the following order: ["Microsoft.AspNetCore.Mvc.Formatters.HttpNoContentOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StringOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StreamOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter"]
[23:10:39 DBG] No information found on request to perform content negotiation.
[23:10:39 DBG] Attempting to select an output formatter without using a content type as no explicit content types were specified for the response.
[23:10:39 DBG] Attempting to select the first formatter in the output formatters list which can write the result.
[23:10:39 DBG] Selected output formatter 'Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter' and content type 'application/json' to write the response.
[23:10:39 INF] Executing OkObjectResult, writing value of type 'AgOpenNtripCaster.Server.Models.DTOs.UserDto'.
[23:10:39 INF] Request starting HTTP/1.1 GET http://localhost:5000/api/users/me - null null
[23:10:39 INF] Executed action AgOpenNtripCaster.Server.Controllers.UsersController.GetCurrentUser (AgOpenNtripCaster.Server) in 1192.001ms
[23:10:39 DBG] 2 candidate(s) found for the request path '/api/users/me'
[23:10:39 INF] Executed endpoint 'AgOpenNtripCaster.Server.Controllers.UsersController.GetCurrentUser (AgOpenNtripCaster.Server)'
[23:10:39 DBG] Endpoint 'AgOpenNtripCaster.Server.Controllers.UsersController.GetCurrentUser (AgOpenNtripCaster.Server)' with route pattern 'api/Users/me' is valid for the request path '/api/users/me'
[23:10:39 INF] HTTP GET /api/users/me responded 200 in 1754.9109 ms
[23:10:39 DBG] Endpoint 'AgOpenNtripCaster.Server.Controllers.UsersController.GetUserById (AgOpenNtripCaster.Server)' with route pattern 'api/Users/{userId}' is valid for the request path '/api/users/me'
[23:10:39 DBG] Connection id "0HNGR4N8NSOJ2" completed keep alive response.
[23:10:39 DBG] Request matched endpoint 'AgOpenNtripCaster.Server.Controllers.UsersController.GetCurrentUser (AgOpenNtripCaster.Server)'
[23:10:39 DBG] 'ApplicationDbContext' disposed.
[23:10:39 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:39 INF] CORS policy execution successful.
[23:10:39 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:39 DBG] Successfully validated the token.
[23:10:39 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (26ms).
[23:10:39 DBG] AuthenticationScheme: Bearer was successfully authenticated.
[23:10:39 INF] Request finished HTTP/1.1 GET http://localhost:5000/api/users/me - 200 null application/json; charset=utf-8 1865.7901ms
[23:10:39 DBG] Authorization was successful.
[23:10:39 INF] Executing endpoint 'AgOpenNtripCaster.Server.Controllers.UsersController.GetCurrentUser (AgOpenNtripCaster.Server)'
[23:10:39 INF] Route matched with {action = "GetCurrentUser", controller = "Users"}. Executing controller action with signature System.Threading.Tasks.Task`1[Microsoft.AspNetCore.Mvc.ActionResult`1[AgOpenNtripCaster.Server.Models.DTOs.UserDto]] GetCurrentUser() on controller AgOpenNtripCaster.Server.Controllers.UsersController (AgOpenNtripCaster.Server).
[23:10:39 DBG] Execution plan of authorization filters (in the following order): ["None"]
[23:10:39 DBG] Execution plan of resource filters (in the following order): ["None"]
[23:10:39 DBG] Execution plan of action filters (in the following order): ["Microsoft.AspNetCore.Mvc.ModelBinding.UnsupportedContentTypeFilter (Order: -3000)", "Microsoft.AspNetCore.Mvc.Infrastructure.ModelStateInvalidFilter (Order: -2000)"]
[23:10:39 DBG] Execution plan of exception filters (in the following order): ["None"]
[23:10:39 DBG] Execution plan of result filters (in the following order): ["Microsoft.AspNetCore.Mvc.Infrastructure.ClientErrorResultFilter (Order: -2000)"]
[23:10:39 DBG] Executing controller factory for controller AgOpenNtripCaster.Server.Controllers.UsersController (AgOpenNtripCaster.Server)
[23:10:39 DBG] Executed controller factory for controller AgOpenNtripCaster.Server.Controllers.UsersController (AgOpenNtripCaster.Server)
[23:10:39 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:39 DBG] Creating DbConnection.
[23:10:39 DBG] Created DbConnection. (4ms).
[23:10:39 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:39 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:39 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:39 DBG] Created DbCommand for 'ExecuteReader' (1ms).
[23:10:39 DBG] Initialized DbCommand for 'ExecuteReader' (3ms).
[23:10:39 DBG] Executing DbCommand [Parameters=[@__userId_0='?'], CommandType='Text', CommandTimeout='30']
SELECT a0."Id", a0."AccessFailedCount", a0."ConcurrencyStamp", a0."CreatedAt", a0."Email", a0."EmailConfirmed", a0."FullName", a0."IsActive", a0."LastGeneratedSourcePassword", a0."LockoutEnabled", a0."LockoutEnd", a0."MaxConnections", a0."NormalizedEmail", a0."NormalizedUserName", a0."PasswordHash", a0."PhoneNumber", a0."PhoneNumberConfirmed", a0."RefreshToken", a0."RefreshTokenExpires", a0."SecurityStamp", a0."SourcePassword", a0."SourcePasswordGeneratedAt", a0."TwoFactorEnabled", a0."UserName", s."GroupsId", s."UsersId", s."Id", s."CreatedAt", s."Description", s."IsActive", s."Name"
FROM (
    SELECT a."Id", a."AccessFailedCount", a."ConcurrencyStamp", a."CreatedAt", a."Email", a."EmailConfirmed", a."FullName", a."IsActive", a."LastGeneratedSourcePassword", a."LockoutEnabled", a."LockoutEnd", a."MaxConnections", a."NormalizedEmail", a."NormalizedUserName", a."PasswordHash", a."PhoneNumber", a."PhoneNumberConfirmed", a."RefreshToken", a."RefreshTokenExpires", a."SecurityStamp", a."SourcePassword", a."SourcePasswordGeneratedAt", a."TwoFactorEnabled", a."UserName"
    FROM "AspNetUsers" AS a
    WHERE a."Id" = @__userId_0
    LIMIT 1
) AS a0
LEFT JOIN (
    SELECT u."GroupsId", u."UsersId", n."Id", n."CreatedAt", n."Description", n."IsActive", n."Name"
    FROM "UserGroups" AS u
    INNER JOIN "NtripGroups" AS n ON u."GroupsId" = n."Id"
) AS s ON a0."Id" = s."UsersId"
ORDER BY a0."Id", s."GroupsId", s."UsersId"
[23:10:40 INF] Executed DbCommand (14ms) [Parameters=[@__userId_0='?'], CommandType='Text', CommandTimeout='30']
SELECT a0."Id", a0."AccessFailedCount", a0."ConcurrencyStamp", a0."CreatedAt", a0."Email", a0."EmailConfirmed", a0."FullName", a0."IsActive", a0."LastGeneratedSourcePassword", a0."LockoutEnabled", a0."LockoutEnd", a0."MaxConnections", a0."NormalizedEmail", a0."NormalizedUserName", a0."PasswordHash", a0."PhoneNumber", a0."PhoneNumberConfirmed", a0."RefreshToken", a0."RefreshTokenExpires", a0."SecurityStamp", a0."SourcePassword", a0."SourcePasswordGeneratedAt", a0."TwoFactorEnabled", a0."UserName", s."GroupsId", s."UsersId", s."Id", s."CreatedAt", s."Description", s."IsActive", s."Name"
FROM (
    SELECT a."Id", a."AccessFailedCount", a."ConcurrencyStamp", a."CreatedAt", a."Email", a."EmailConfirmed", a."FullName", a."IsActive", a."LastGeneratedSourcePassword", a."LockoutEnabled", a."LockoutEnd", a."MaxConnections", a."NormalizedEmail", a."NormalizedUserName", a."PasswordHash", a."PhoneNumber", a."PhoneNumberConfirmed", a."RefreshToken", a."RefreshTokenExpires", a."SecurityStamp", a."SourcePassword", a."SourcePasswordGeneratedAt", a."TwoFactorEnabled", a."UserName"
    FROM "AspNetUsers" AS a
    WHERE a."Id" = @__userId_0
    LIMIT 1
) AS a0
LEFT JOIN (
    SELECT u."GroupsId", u."UsersId", n."Id", n."CreatedAt", n."Description", n."IsActive", n."Name"
    FROM "UserGroups" AS u
    INNER JOIN "NtripGroups" AS n ON u."GroupsId" = n."Id"
) AS s ON a0."Id" = s."UsersId"
ORDER BY a0."Id", s."GroupsId", s."UsersId"
[23:10:40 DBG] Context 'ApplicationDbContext' started tracking 'NtripUser' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:40 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:40 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 15ms reading results.
[23:10:40 INF] Request starting HTTP/1.1 OPTIONS http://localhost:5000/api/ntrip-hub/negotiate?negotiateVersion=1 - null null
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ3" accepted.
[23:10:40 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/ntrip-hub/negotiate'
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ3" started.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ4" accepted.
[23:10:40 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (21ms).
[23:10:40 DBG] Request matched endpoint '/api/ntrip-hub/negotiate'
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ5" accepted.
[23:10:40 INF] Request starting HTTP/1.1 OPTIONS http://localhost:5000/api/ntrip-hub/negotiate?negotiateVersion=1 - null null
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ6" accepted.
[23:10:40 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ4" started.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ5" started.
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/ntrip-hub/negotiate'
[23:10:40 INF] CORS policy execution successful.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ6" started.
[23:10:40 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:40 INF] Request starting HTTP/1.1 OPTIONS http://localhost:5000/api/admin/config/stats - null null
[23:10:40 INF] Request starting HTTP/1.1 OPTIONS http://localhost:5000/api/activity/recent?limit=50 - null null
[23:10:40 DBG] The request is a preflight request.
[23:10:40 DBG] Request matched endpoint '/api/ntrip-hub/negotiate'
[23:10:40 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/admin/config/stats'
[23:10:40 INF] Request starting HTTP/1.1 OPTIONS http://localhost:5000/api/mountpoints?page=1&pageSize=100 - null null
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/activity/recent'
[23:10:40 INF] HTTP OPTIONS /api/ntrip-hub/negotiate responded 204 in 121.0017 ms
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 DBG] Created DbCommand for 'ExecuteReader' (64ms).
[23:10:40 DBG] Request matched endpoint '405 HTTP Method Not Supported'
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/mountpoints'
[23:10:40 DBG] Request matched endpoint '405 HTTP Method Not Supported'
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ2" completed keep alive response.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ7" accepted.
[23:10:40 INF] CORS policy execution successful.
[23:10:40 DBG] Initialized DbCommand for 'ExecuteReader' (92ms).
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 DBG] Request matched endpoint '405 HTTP Method Not Supported'
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 INF] Request finished HTTP/1.1 OPTIONS http://localhost:5000/api/ntrip-hub/negotiate?negotiateVersion=1 - 204 null null 239.8871ms
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ7" started.
[23:10:40 DBG] The request is a preflight request.
[23:10:40 DBG] Executing DbCommand [Parameters=[@__userId_0='?'], CommandType='Text', CommandTimeout='30']
SELECT a0."Name"
FROM "AspNetUserRoles" AS a
INNER JOIN "AspNetRoles" AS a0 ON a."RoleId" = a0."Id"
WHERE a."UserId" = @__userId_0
[23:10:40 INF] CORS policy execution successful.
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 INF] CORS policy execution successful.
[23:10:40 INF] Request starting HTTP/1.1 OPTIONS http://localhost:5000/api/admin/config/stats - null null
[23:10:40 INF] Request starting HTTP/1.1 POST http://localhost:5000/api/ntrip-hub/negotiate?negotiateVersion=1 - null 0
[23:10:40 INF] HTTP OPTIONS /api/ntrip-hub/negotiate responded 204 in 82.9802 ms
[23:10:40 DBG] The request is a preflight request.
[23:10:40 INF] Executed DbCommand (37ms) [Parameters=[@__userId_0='?'], CommandType='Text', CommandTimeout='30']
SELECT a0."Name"
FROM "AspNetUserRoles" AS a
INNER JOIN "AspNetRoles" AS a0 ON a."RoleId" = a0."Id"
WHERE a."UserId" = @__userId_0
[23:10:40 INF] CORS policy execution successful.
[23:10:40 DBG] The request is a preflight request.
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/admin/config/stats'
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/ntrip-hub/negotiate'
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ3" completed keep alive response.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ8" accepted.
[23:10:40 INF] HTTP OPTIONS /api/admin/config/stats responded 204 in 90.7342 ms
[23:10:40 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:40 DBG] The request is a preflight request.
[23:10:40 INF] HTTP OPTIONS /api/activity/recent responded 204 in 110.8009 ms
[23:10:40 DBG] Request matched endpoint '405 HTTP Method Not Supported'
[23:10:40 DBG] Request matched endpoint '/api/ntrip-hub/negotiate'
[23:10:40 INF] Request finished HTTP/1.1 OPTIONS http://localhost:5000/api/ntrip-hub/negotiate?negotiateVersion=1 - 204 null null 318.1658ms
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ8" started.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ4" completed keep alive response.
[23:10:40 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 32ms reading results.
[23:10:40 INF] HTTP OPTIONS /api/mountpoints responded 204 in 114.4683 ms
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ5" completed keep alive response.
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 INF] Request starting HTTP/1.1 OPTIONS http://localhost:5000/api/activity/recent?limit=50 - null null
[23:10:40 INF] Request starting HTTP/1.1 POST http://localhost:5000/api/ntrip-hub/negotiate?negotiateVersion=1 - null 0
[23:10:40 INF] Request finished HTTP/1.1 OPTIONS http://localhost:5000/api/admin/config/stats - 204 null null 283.9234ms
[23:10:40 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ6" completed keep alive response.
[23:10:40 INF] Request finished HTTP/1.1 OPTIONS http://localhost:5000/api/activity/recent?limit=50 - 204 null null 291.7889ms
[23:10:40 INF] CORS policy execution successful.
[23:10:40 INF] CORS policy execution successful.
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/activity/recent'
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/ntrip-hub/negotiate'
[23:10:40 INF] Request starting HTTP/1.1 OPTIONS http://localhost:5000/api/mountpoints?page=1&pageSize=100 - null null
[23:10:40 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (39ms).
[23:10:40 INF] Request finished HTTP/1.1 OPTIONS http://localhost:5000/api/mountpoints?page=1&pageSize=100 - 204 null null 315.5553ms
[23:10:40 INF] Request starting HTTP/1.1 GET http://localhost:5000/api/activity/recent?limit=50 - null null
[23:10:40 DBG] The request is a preflight request.
[23:10:40 DBG] Request matched endpoint '405 HTTP Method Not Supported'
[23:10:40 DBG] AuthenticationScheme: Bearer was not authenticated.
[23:10:40 DBG] Request matched endpoint '/api/ntrip-hub/negotiate'
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/mountpoints'
[23:10:40 DBG] List of registered output formatters, in the following order: ["Microsoft.AspNetCore.Mvc.Formatters.HttpNoContentOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StringOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StreamOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter"]
[23:10:40 INF] Request starting HTTP/1.1 GET http://localhost:5000/api/admin/config/stats - null null
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/activity/recent'
[23:10:40 INF] HTTP OPTIONS /api/admin/config/stats responded 204 in 109.3152 ms
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 INF] Executing endpoint '/api/ntrip-hub/negotiate'
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 DBG] Request matched endpoint '405 HTTP Method Not Supported'
[23:10:40 DBG] No information found on request to perform content negotiation.
[23:10:40 DBG] Attempting to select an output formatter without using a content type as no explicit content types were specified for the response.
[23:10:40 DBG] Endpoint 'AgOpenNtripCaster.Server.Controllers.ActivityController.GetRecentActivities (AgOpenNtripCaster.Server)' with route pattern 'api/activity/recent' is valid for the request path '/api/activity/recent'
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ2" completed keep alive response.
[23:10:40 INF] CORS policy execution successful.
[23:10:40 INF] CORS policy execution successful.
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/admin/config/stats'
[23:10:40 DBG] New connection fsGTL7JGJHuqPVOr0OQRGg created.
[23:10:40 DBG] Attempting to select the first formatter in the output formatters list which can write the result.
[23:10:40 DBG] Request matched endpoint 'AgOpenNtripCaster.Server.Controllers.ActivityController.GetRecentActivities (AgOpenNtripCaster.Server)'
[23:10:40 INF] Request finished HTTP/1.1 OPTIONS http://localhost:5000/api/admin/config/stats - 204 null null 341.3081ms
[23:10:40 DBG] The request is a preflight request.
[23:10:40 DBG] AuthenticationScheme: Bearer was not authenticated.
[23:10:40 INF] CORS policy execution successful.
[23:10:40 DBG] Endpoint 'AgOpenNtripCaster.Server.Controllers.AdminConfigController.GetDashboardStats (AgOpenNtripCaster.Server)' with route pattern 'api/admin/config/stats' is valid for the request path '/api/admin/config/stats'
[23:10:40 DBG] Selected output formatter 'Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter' and content type 'application/json' to write the response.
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 INF] HTTP OPTIONS /api/activity/recent responded 204 in 139.0921 ms
[23:10:40 INF] Request starting HTTP/1.1 GET http://localhost:5000/api/mountpoints?page=1&pageSize=100 - null null
[23:10:40 INF] Executing endpoint '/api/ntrip-hub/negotiate'
[23:10:40 DBG] The request is a preflight request.
[23:10:40 DBG] Request matched endpoint 'AgOpenNtripCaster.Server.Controllers.AdminConfigController.GetDashboardStats (AgOpenNtripCaster.Server)'
[23:10:40 INF] Executing OkObjectResult, writing value of type 'AgOpenNtripCaster.Server.Models.DTOs.UserDto'.
[23:10:40 INF] CORS policy execution successful.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ3" completed keep alive response.
[23:10:40 DBG] Sending negotiation response.
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/mountpoints'
[23:10:40 DBG] New connection pOuJYGloPgBXJ_E6StVhlA created.
[23:10:40 INF] HTTP OPTIONS /api/mountpoints responded 204 in 90.5955 ms
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 INF] Executed action AgOpenNtripCaster.Server.Controllers.UsersController.GetCurrentUser (AgOpenNtripCaster.Server) in 761.2198ms
[23:10:40 DBG] Successfully validated the token.
[23:10:40 INF] Request finished HTTP/1.1 OPTIONS http://localhost:5000/api/activity/recent?limit=50 - 204 null null 295.3329ms
[23:10:40 INF] Executed endpoint '/api/ntrip-hub/negotiate'
[23:10:40 DBG] Endpoint 'AgOpenNtripCaster.Server.Controllers.MountPointsController.GetMountPoints (AgOpenNtripCaster.Server)' with route pattern 'api/MountPoints' is valid for the request path '/api/mountpoints'
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ4" completed keep alive response.
[23:10:40 INF] CORS policy execution successful.
[23:10:40 INF] Executed endpoint 'AgOpenNtripCaster.Server.Controllers.UsersController.GetCurrentUser (AgOpenNtripCaster.Server)'
[23:10:40 DBG] AuthenticationScheme: Bearer was successfully authenticated.
[23:10:40 INF] HTTP POST /api/ntrip-hub/negotiate responded 200 in 345.7018 ms
[23:10:40 DBG] Sending negotiation response.
[23:10:40 DBG] Request matched endpoint 'AgOpenNtripCaster.Server.Controllers.MountPointsController.GetMountPoints (AgOpenNtripCaster.Server)'
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ9" accepted.
[23:10:40 INF] Request finished HTTP/1.1 OPTIONS http://localhost:5000/api/mountpoints?page=1&pageSize=100 - 204 null null 307.9608ms
[23:10:40 DBG] Successfully validated the token.
[23:10:40 INF] HTTP GET /api/users/me responded 200 in 972.8119 ms
[23:10:40 DBG] Authorization was successful.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ7" completed keep alive response.
[23:10:40 INF] Executed endpoint '/api/ntrip-hub/negotiate'
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ9" started.
[23:10:40 DBG] AuthenticationScheme: Bearer was successfully authenticated.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ1" completed keep alive response.
[23:10:40 INF] Executing endpoint 'AgOpenNtripCaster.Server.Controllers.ActivityController.GetRecentActivities (AgOpenNtripCaster.Server)'
[23:10:40 INF] Request finished HTTP/1.1 POST http://localhost:5000/api/ntrip-hub/negotiate?negotiateVersion=1 - 200 316 application/json 561.9862ms
[23:10:40 INF] HTTP POST /api/ntrip-hub/negotiate responded 200 in 325.1384 ms
[23:10:40 INF] CORS policy execution successful.
[23:10:40 INF] Request starting HTTP/1.1 GET http://localhost:5000/api/ntrip-hub?id=fMZtXZ-JEsmWUmKgRpLd5g - null null
[23:10:40 DBG] Authorization was successful.
[23:10:40 DBG] 'ApplicationDbContext' disposed.
[23:10:40 DBG] Connection id "0HNGR4N8NSOJ8" completed keep alive response.
[23:10:40 DBG] Successfully validated the token.
[23:10:40 INF] Route matched with {action = "GetRecentActivities", controller = "Activity"}. Executing controller action with signature System.Threading.Tasks.Task`1[Microsoft.AspNetCore.Mvc.ActionResult`1[System.Collections.Generic.List`1[AgOpenNtripCaster.Server.Services.NTRIP.ActivityDto]]] GetRecentActivities(Int32) on controller AgOpenNtripCaster.Server.Controllers.ActivityController (AgOpenNtripCaster.Server).
[23:10:40 DBG] 1 candidate(s) found for the request path '/api/ntrip-hub'
[23:10:40 INF] Executing endpoint 'AgOpenNtripCaster.Server.Controllers.AdminConfigController.GetDashboardStats (AgOpenNtripCaster.Server)'
[23:10:40 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:40 INF] Request finished HTTP/1.1 POST http://localhost:5000/api/ntrip-hub/negotiate?negotiateVersion=1 - 200 316 application/json 499.3104ms
[23:10:40 DBG] AuthenticationScheme: Bearer was successfully authenticated.
[23:10:40 DBG] Execution plan of authorization filters (in the following order): ["None"]
[23:10:40 DBG] Request matched endpoint '/api/ntrip-hub'
[23:10:40 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (27ms).
[23:10:40 INF] Route matched with {action = "GetDashboardStats", controller = "AdminConfig"}. Executing controller action with signature System.Threading.Tasks.Task`1[Microsoft.AspNetCore.Mvc.ActionResult`1[AgOpenNtripCaster.Server.Models.DTOs.DashboardStatsDto]] GetDashboardStats() on controller AgOpenNtripCaster.Server.Controllers.AdminConfigController (AgOpenNtripCaster.Server).
[23:10:40 INF] Executing endpoint 'AgOpenNtripCaster.Server.Controllers.MountPointsController.GetMountPoints (AgOpenNtripCaster.Server)'
[23:10:40 DBG] Execution plan of resource filters (in the following order): ["None"]
[23:10:40 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:41 INF] Request finished HTTP/1.1 GET http://localhost:5000/api/users/me - 200 null application/json; charset=utf-8 1225.453ms
[23:10:41 DBG] Execution plan of authorization filters (in the following order): ["None"]
[23:10:41 DBG] Execution plan of action filters (in the following order): ["Microsoft.AspNetCore.Mvc.ModelBinding.UnsupportedContentTypeFilter (Order: -3000)", "Microsoft.AspNetCore.Mvc.Infrastructure.ModelStateInvalidFilter (Order: -2000)"]
[23:10:41 INF] Route matched with {action = "GetMountPoints", controller = "MountPoints"}. Executing controller action with signature System.Threading.Tasks.Task`1[Microsoft.AspNetCore.Mvc.ActionResult`1[AgOpenNtripCaster.Server.Models.DTOs.MountPointListResponse]] GetMountPoints(Int32, Int32) on controller AgOpenNtripCaster.Server.Controllers.MountPointsController (AgOpenNtripCaster.Server).
[23:10:41 INF] CORS policy execution successful.
[23:10:41 DBG] Execution plan of resource filters (in the following order): ["None"]
[23:10:41 DBG] Execution plan of exception filters (in the following order): ["None"]
[23:10:41 DBG] Execution plan of authorization filters (in the following order): ["None"]
[23:10:41 DBG] AuthenticationScheme: Bearer was not authenticated.
[23:10:41 DBG] Execution plan of action filters (in the following order): ["Microsoft.AspNetCore.Mvc.ModelBinding.UnsupportedContentTypeFilter (Order: -3000)", "Microsoft.AspNetCore.Mvc.Infrastructure.ModelStateInvalidFilter (Order: -2000)"]
[23:10:41 DBG] Execution plan of result filters (in the following order): ["Microsoft.AspNetCore.Mvc.Infrastructure.ClientErrorResultFilter (Order: -2000)"]
[23:10:41 DBG] Execution plan of resource filters (in the following order): ["None"]
[23:10:41 INF] Executing endpoint '/api/ntrip-hub'
[23:10:41 DBG] Execution plan of exception filters (in the following order): ["None"]
[23:10:41 DBG] Executing controller factory for controller AgOpenNtripCaster.Server.Controllers.ActivityController (AgOpenNtripCaster.Server)
[23:10:41 DBG] Execution plan of action filters (in the following order): ["Microsoft.AspNetCore.Mvc.ModelBinding.UnsupportedContentTypeFilter (Order: -3000)", "Microsoft.AspNetCore.Mvc.Infrastructure.ModelStateInvalidFilter (Order: -2000)"]
[23:10:41 DBG] Execution plan of result filters (in the following order): ["Microsoft.AspNetCore.Mvc.Infrastructure.ClientErrorResultFilter (Order: -2000)"]
[23:10:41 DBG] Establishing new connection.
[23:10:41 DBG] Executed controller factory for controller AgOpenNtripCaster.Server.Controllers.ActivityController (AgOpenNtripCaster.Server)
[23:10:41 DBG] Execution plan of exception filters (in the following order): ["None"]
[23:10:41 DBG] Executing controller factory for controller AgOpenNtripCaster.Server.Controllers.AdminConfigController (AgOpenNtripCaster.Server)
[23:10:41 DBG] Execution plan of result filters (in the following order): ["Microsoft.AspNetCore.Mvc.Infrastructure.ClientErrorResultFilter (Order: -2000)"]
[23:10:41 DBG] OnConnectedAsync started.
[23:10:41 DBG] Attempting to bind parameter 'limit' of type 'System.Int32' ...
[23:10:41 DBG] Executed controller factory for controller AgOpenNtripCaster.Server.Controllers.AdminConfigController (AgOpenNtripCaster.Server)
[23:10:41 DBG] Connection id "0HNGR4N8NSOJA" accepted.
[23:10:41 DBG] Executing controller factory for controller AgOpenNtripCaster.Server.Controllers.MountPointsController (AgOpenNtripCaster.Server)
[23:10:41 DBG] Socket opened using Sub-Protocol: 'null'.
[23:10:41 DBG] Attempting to bind parameter 'limit' of type 'System.Int32' using the name 'limit' in request data ...
[23:10:41 DBG] Connection id "0HNGR4N8NSOJA" started.
[23:10:41 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:41 DBG] Executed controller factory for controller AgOpenNtripCaster.Server.Controllers.MountPointsController (AgOpenNtripCaster.Server)
[23:10:41 DBG] Done attempting to bind parameter 'limit' of type 'System.Int32'.
[23:10:41 INF] Request starting HTTP/1.1 GET http://localhost:5000/api/ntrip-hub?id=c4l-1pSdJERcfsvnnvRXgA - null null
[23:10:41 DBG] Creating DbConnection.
[23:10:41 DBG] Found protocol implementation for requested protocol: json.
[23:10:41 DBG] Attempting to bind parameter 'page' of type 'System.Int32' ...
[23:10:41 DBG] Done attempting to bind parameter 'limit' of type 'System.Int32'.
[23:10:41 DBG] 1 candidate(s) found for the request path '/api/ntrip-hub'
[23:10:41 DBG] Created DbConnection. (20ms).
[23:10:41 DBG] Attempting to bind parameter 'page' of type 'System.Int32' using the name 'page' in request data ...
[23:10:41 DBG] Completed connection handshake. Using HubProtocol 'json'.
[23:10:41 DBG] Attempting to validate the bound parameter 'limit' of type 'System.Int32' ...
[23:10:41 DBG] Request matched endpoint '/api/ntrip-hub'
[23:10:41 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:41 DBG] Done attempting to bind parameter 'page' of type 'System.Int32'.
[23:10:41 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:41 DBG] Done attempting to validate the bound parameter 'limit' of type 'System.Int32'.
[23:10:41 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:41 INF] SignalR client connected: fsGTL7JGJHuqPVOr0OQRGg
[23:10:41 DBG] Done attempting to bind parameter 'page' of type 'System.Int32'.
[23:10:41 INF] CORS policy execution successful.
[23:10:41 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:41 DBG] Attempting to validate the bound parameter 'page' of type 'System.Int32' ...
[23:10:41 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:41 DBG] AuthenticationScheme: Bearer was not authenticated.
[23:10:41 DBG] Created DbCommand for 'ExecuteReader' (14ms).
[23:10:41 DBG] Done attempting to validate the bound parameter 'page' of type 'System.Int32'.
[23:10:41 DBG] Compiling query expression:
'DbSet<Activity>()
    .Include(a => a.MountPoint)
    .Include(a => a.User)
    .OrderByDescending(a => a.CreatedAt)
    .Take(__p_0)'
[23:10:41 INF] Executing endpoint '/api/ntrip-hub'
[23:10:41 DBG] Initialized DbCommand for 'ExecuteReader' (30ms).
[23:10:41 DBG] Attempting to bind parameter 'pageSize' of type 'System.Int32' ...
[23:10:41 DBG] Establishing new connection.
[23:10:41 DBG] Including navigation: 'Activity.MountPoint'.
[23:10:41 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:41 DBG] Attempting to bind parameter 'pageSize' of type 'System.Int32' using the name 'pageSize' in request data ...
[23:10:41 DBG] OnConnectedAsync started.
[23:10:41 DBG] Socket opened using Sub-Protocol: 'null'.
[23:10:41 DBG] Including navigation: 'Activity.User'.
[23:10:41 DBG] Done attempting to bind parameter 'pageSize' of type 'System.Int32'.
[23:10:41 INF] Executed DbCommand (25ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:41 DBG] Found protocol implementation for requested protocol: json.
[23:10:41 DBG] Done attempting to bind parameter 'pageSize' of type 'System.Int32'.
[23:10:41 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:41 DBG] Completed connection handshake. Using HubProtocol 'json'.
[23:10:41 DBG] Attempting to validate the bound parameter 'pageSize' of type 'System.Int32' ...
[23:10:41 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 13ms reading results.
[23:10:41 INF] SignalR client connected: pOuJYGloPgBXJ_E6StVhlA
[23:10:41 DBG] Done attempting to validate the bound parameter 'pageSize' of type 'System.Int32'.
[23:10:41 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:41 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (10ms).
[23:10:41 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:41 DBG] Generated query execution expression:
'queryContext => SingleQueryingEnumerable.Create<Activity>(
    relationalQueryContext: (RelationalQueryContext)queryContext,
    relationalCommandResolver: parameters => [LIFTABLE Constant: RelationalCommandCache.QueryExpression(
        Client Projections:
            0 -> Dictionary<IProperty, int> { [Property: Activity.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 0], [Property: Activity.CreatedAt (DateTime) Required, 1], [Property: Activity.Description (string) Required, 2], [Property: Activity.LogLevel (string) Required, 3], [Property: Activity.MountPointId (int?) FK Index, 4], [Property: Activity.Type (ActivityType) Required, 5], [Property: Activity.UserId (string) FK Index, 6] }
            1 -> Dictionary<IProperty, int> { [Property: MountPoint.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 7], [Property: MountPoint.BytesPerSecond (int?), 8], [Property: MountPoint.CreatedAt (DateTime) Required, 9], [Property: MountPoint.Description (string) Required, 10], [Property: MountPoint.DetectedFormat (string), 11], [Property: MountPoint.DetectedNavSystems (string), 12], [Property: MountPoint.Format (string) Required, 13], [Property: MountPoint.IsActive (bool) Required, 14], [Property: MountPoint.LastRtcmMessageTime (DateTime?), 15], [Property: MountPoint.Latitude (decimal?), 16], [Property: MountPoint.Longitude (decimal?), 17], [Property: MountPoint.MaxClients (int) Required, 18], [Property: MountPoint.MessageCount (int) Required, 19], [Property: MountPoint.Name (string) Required, 20], [Property: MountPoint.OwnerId (no field, string) Shadow FK Index, 21], [Property: MountPoint.ReferenceStationId (int?), 22], [Property: MountPoint.RequireClientAuthentication (bool) Required, 23], [Property: MountPoint.RtcmLatitude (decimal?), 24], [Property: MountPoint.RtcmLongitude (decimal?), 25], [Property: MountPoint.SourcePassword (string) Required, 26], [Property: MountPoint.UserId (string), 27] }
            2 -> Dictionary<IProperty, int> { [Property: NtripUser.Id (string) Required PK AfterSave:Throw, 28], [Property: NtripUser.AccessFailedCount (int) Required, 29], [Property: NtripUser.ConcurrencyStamp (string) Concurrency, 30], [Property: NtripUser.CreatedAt (DateTime) Required, 31], [Property: NtripUser.Email (string) MaxLength(256), 32], [Property: NtripUser.EmailConfirmed (bool) Required, 33], [Property: NtripUser.FullName (string) Required, 34], [Property: NtripUser.IsActive (bool) Required, 35], [Property: NtripUser.LastGeneratedSourcePassword (string), 36], [Property: NtripUser.LockoutEnabled (bool) Required, 37], [Property: NtripUser.LockoutEnd (DateTimeOffset?), 38], [Property: NtripUser.MaxConnections (int) Required, 39], [Property: NtripUser.NormalizedEmail (string) Index MaxLength(256), 40], [Property: NtripUser.NormalizedUserName (string) Index MaxLength(256), 41], [Property: NtripUser.PasswordHash (string), 42], [Property: NtripUser.PhoneNumber (string), 43], [Property: NtripUser.PhoneNumberConfirmed (bool) Required, 44], [Property: NtripUser.RefreshToken (string), 45], [Property: NtripUser.RefreshTokenExpires (DateTime?), 46], [Property: NtripUser.SecurityStamp (string), 47], [Property: NtripUser.SourcePassword (string), 48], [Property: NtripUser.SourcePasswordGeneratedAt (DateTime?), 49], [Property: NtripUser.TwoFactorEnabled (bool) Required, 50], [Property: NtripUser.UserName (string) MaxLength(256), 51] }
        SELECT a0.Id, a0.CreatedAt, a0.Description, a0.LogLevel, a0.MountPointId, a0.Type, a0.UserId, m.Id, m.BytesPerSecond, m.CreatedAt, m.Description, m.DetectedFormat, m.DetectedNavSystems, m.Format, m.IsActive, m.LastRtcmMessageTime, m.Latitude, m.Longitude, m.MaxClients, m.MessageCount, m.Name, m.OwnerId, m.ReferenceStationId, m.RequireClientAuthentication, m.RtcmLatitude, m.RtcmLongitude, m.SourcePassword, m.UserId, a1.Id, a1.AccessFailedCount, a1.ConcurrencyStamp, a1.CreatedAt, a1.Email, a1.EmailConfirmed, a1.FullName, a1.IsActive, a1.LastGeneratedSourcePassword, a1.LockoutEnabled, a1.LockoutEnd, a1.MaxConnections, a1.NormalizedEmail, a1.NormalizedUserName, a1.PasswordHash, a1.PhoneNumber, a1.PhoneNumberConfirmed, a1.RefreshToken, a1.RefreshTokenExpires, a1.SecurityStamp, a1.SourcePassword, a1.SourcePasswordGeneratedAt, a1.TwoFactorEnabled, a1.UserName
        FROM
        (
            SELECT TOP(@__p_0) a.Id, a.CreatedAt, a.Description, a.LogLevel, a.MountPointId, a.Type, a.UserId
            FROM Activities AS a
            ORDER BY a.CreatedAt DESC
        ) AS a0
        LEFT JOIN MountPoints AS m ON a0.MountPointId == m.Id
        LEFT JOIN AspNetUsers AS a1 ON a0.UserId == a1.Id
        ORDER BY a0.CreatedAt DESC) | Resolver: c => new RelationalCommandCache(
        c.Dependencies.MemoryCache,
        c.RelationalDependencies.QuerySqlGeneratorFactory,
        c.RelationalDependencies.RelationalParameterBasedSqlProcessorFactory,
        Client Projections:
            0 -> Dictionary<IProperty, int> { [Property: Activity.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 0], [Property: Activity.CreatedAt (DateTime) Required, 1], [Property: Activity.Description (string) Required, 2], [Property: Activity.LogLevel (string) Required, 3], [Property: Activity.MountPointId (int?) FK Index, 4], [Property: Activity.Type (ActivityType) Required, 5], [Property: Activity.UserId (string) FK Index, 6] }
            1 -> Dictionary<IProperty, int> { [Property: MountPoint.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 7], [Property: MountPoint.BytesPerSecond (int?), 8], [Property: MountPoint.CreatedAt (DateTime) Required, 9], [Property: MountPoint.Description (string) Required, 10], [Property: MountPoint.DetectedFormat (string), 11], [Property: MountPoint.DetectedNavSystems (string), 12], [Property: MountPoint.Format (string) Required, 13], [Property: MountPoint.IsActive (bool) Required, 14], [Property: MountPoint.LastRtcmMessageTime (DateTime?), 15], [Property: MountPoint.Latitude (decimal?), 16], [Property: MountPoint.Longitude (decimal?), 17], [Property: MountPoint.MaxClients (int) Required, 18], [Property: MountPoint.MessageCount (int) Required, 19], [Property: MountPoint.Name (string) Required, 20], [Property: MountPoint.OwnerId (no field, string) Shadow FK Index, 21], [Property: MountPoint.ReferenceStationId (int?), 22], [Property: MountPoint.RequireClientAuthentication (bool) Required, 23], [Property: MountPoint.RtcmLatitude (decimal?), 24], [Property: MountPoint.RtcmLongitude (decimal?), 25], [Property: MountPoint.SourcePassword (string) Required, 26], [Property: MountPoint.UserId (string), 27] }
            2 -> Dictionary<IProperty, int> { [Property: NtripUser.Id (string) Required PK AfterSave:Throw, 28], [Property: NtripUser.AccessFailedCount (int) Required, 29], [Property: NtripUser.ConcurrencyStamp (string) Concurrency, 30], [Property: NtripUser.CreatedAt (DateTime) Required, 31], [Property: NtripUser.Email (string) MaxLength(256), 32], [Property: NtripUser.EmailConfirmed (bool) Required, 33], [Property: NtripUser.FullName (string) Required, 34], [Property: NtripUser.IsActive (bool) Required, 35], [Property: NtripUser.LastGeneratedSourcePassword (string), 36], [Property: NtripUser.LockoutEnabled (bool) Required, 37], [Property: NtripUser.LockoutEnd (DateTimeOffset?), 38], [Property: NtripUser.MaxConnections (int) Required, 39], [Property: NtripUser.NormalizedEmail (string) Index MaxLength(256), 40], [Property: NtripUser.NormalizedUserName (string) Index MaxLength(256), 41], [Property: NtripUser.PasswordHash (string), 42], [Property: NtripUser.PhoneNumber (string), 43], [Property: NtripUser.PhoneNumberConfirmed (bool) Required, 44], [Property: NtripUser.RefreshToken (string), 45], [Property: NtripUser.RefreshTokenExpires (DateTime?), 46], [Property: NtripUser.SecurityStamp (string), 47], [Property: NtripUser.SourcePassword (string), 48], [Property: NtripUser.SourcePasswordGeneratedAt (DateTime?), 49], [Property: NtripUser.TwoFactorEnabled (bool) Required, 50], [Property: NtripUser.UserName (string) MaxLength(256), 51] }
        SELECT a0.Id, a0.CreatedAt, a0.Description, a0.LogLevel, a0.MountPointId, a0.Type, a0.UserId, m.Id, m.BytesPerSecond, m.CreatedAt, m.Description, m.DetectedFormat, m.DetectedNavSystems, m.Format, m.IsActive, m.LastRtcmMessageTime, m.Latitude, m.Longitude, m.MaxClients, m.MessageCount, m.Name, m.OwnerId, m.ReferenceStationId, m.RequireClientAuthentication, m.RtcmLatitude, m.RtcmLongitude, m.SourcePassword, m.UserId, a1.Id, a1.AccessFailedCount, a1.ConcurrencyStamp, a1.CreatedAt, a1.Email, a1.EmailConfirmed, a1.FullName, a1.IsActive, a1.LastGeneratedSourcePassword, a1.LockoutEnabled, a1.LockoutEnd, a1.MaxConnections, a1.NormalizedEmail, a1.NormalizedUserName, a1.PasswordHash, a1.PhoneNumber, a1.PhoneNumberConfirmed, a1.RefreshToken, a1.RefreshTokenExpires, a1.SecurityStamp, a1.SourcePassword, a1.SourcePasswordGeneratedAt, a1.TwoFactorEnabled, a1.UserName
        FROM
        (
            SELECT TOP(@__p_0) a.Id, a.CreatedAt, a.Description, a.LogLevel, a.MountPointId, a.Type, a.UserId
            FROM Activities AS a
            ORDER BY a.CreatedAt DESC
        ) AS a0
        LEFT JOIN MountPoints AS m ON a0.MountPointId == m.Id
        LEFT JOIN AspNetUsers AS a1 ON a0.UserId == a1.Id
        ORDER BY a0.CreatedAt DESC,
        False,
        new HashSet<string>(
            new string[]{ },
            StringComparer.Ordinal
        )
    )].GetRelationalCommandTemplate(parameters),
    readerColumns: null,
    shaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
    {
        Activity entity;
        MountPoint entity;
        NtripUser entity;
        entity =
        {
            MaterializationContext materializationContext1;
            IEntityType entityType1;
            Activity instance1;
            InternalEntityEntry entry1;
            bool hasNullKey1;
            materializationContext1 = new MaterializationContext(
                [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                queryContext.Context
            );
            instance1 = default(Activity);
            entry1 = queryContext.TryGetEntry(
                key: [LIFTABLE Constant: Key: Activity.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.Activity").FindPrimaryKey()],
                keyValues: new object[]{ (object)dataReader.GetInt32(0) },
                throwOnNullKey: True,
                hasNullKey: hasNullKey1);
            !(hasNullKey1) ? entry1 != default(InternalEntityEntry) ?
            {
                entityType1 = entry1.EntityType;
                return instance1 = (Activity)entry1.Entity;
            } :
            {
                ISnapshot shadowSnapshot1;
                shadowSnapshot1 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                entityType1 = [LIFTABLE Constant: EntityType: Activity | Resolver: namelessParameter{0} => namelessParameter{0}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.Activity")];
                instance1 = switch (entityType1)
                {
                    case [LIFTABLE Constant: EntityType: Activity | Resolver: namelessParameter{1} => namelessParameter{1}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.Activity")]:
                        {
                            return
                            {
                                Activity instance;
                                instance = new Activity();
                                instance.<Id>k__BackingField = dataReader.GetInt32(0);
                                instance.<CreatedAt>k__BackingField = dataReader.GetDateTime(1);
                                instance.<Description>k__BackingField = dataReader.GetString(2);
                                instance.<LogLevel>k__BackingField = dataReader.GetString(3);
                                instance.<MountPointId>k__BackingField = dataReader.IsDBNull(4) ? default(int?) : (int?)dataReader.GetInt32(4);
                                instance.<Type>k__BackingField = Invoke(((EnumToNumberConverter<ActivityType, int>)((IReadOnlyProperty)[LIFTABLE Constant: Property: Activity.Type (ActivityType) Required | Resolver: namelessParameter{2} => namelessParameter{2}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.Activity").FindProperty("Type")]).GetTypeMapping().Converter).ConvertFromProviderTyped, dataReader.GetInt32(5));
                                instance.<UserId>k__BackingField = dataReader.IsDBNull(6) ? default(string) : dataReader.GetString(6);
                                (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                    context: materializationContext1.Context,
                                    entity: instance,
                                    queryTrackingBehavior: TrackAll,
                                    structuralType: [LIFTABLE Constant: EntityType: Activity | Resolver: namelessParameter{3} => namelessParameter{3}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.Activity")]) : default(void);
                                return instance;
                            }}
                    default:
                        default(Activity)
                }
                ;
                entry1 = entityType1 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                    entityType: entityType1,
                    entity: instance1,
                    snapshot: shadowSnapshot1);
                return instance1;
            } : default(void);
            return instance1;
        };
        entity =
        {
            MaterializationContext materializationContext2;
            IEntityType entityType2;
            MountPoint instance2;
            InternalEntityEntry entry2;
            bool hasNullKey2;
            materializationContext2 = new MaterializationContext(
                [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                queryContext.Context
            );
            instance2 = default(MountPoint);
            entry2 = queryContext.TryGetEntry(
                key: [LIFTABLE Constant: Key: MountPoint.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint").FindPrimaryKey()],
                keyValues: new object[]{ dataReader.IsDBNull(7) ? default(object) : (object)dataReader.GetInt32(7) },
                throwOnNullKey: False,
                hasNullKey: hasNullKey2);
            !(hasNullKey2) ? entry2 != default(InternalEntityEntry) ?
            {
                entityType2 = entry2.EntityType;
                return instance2 = (MountPoint)entry2.Entity;
            } :
            {
                ISnapshot shadowSnapshot2;
                shadowSnapshot2 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                entityType2 = [LIFTABLE Constant: EntityType: MountPoint | Resolver: namelessParameter{4} => namelessParameter{4}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint")];
                instance2 = switch (entityType2)
                {
                    case [LIFTABLE Constant: EntityType: MountPoint | Resolver: namelessParameter{5} => namelessParameter{5}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint")]:
                        {
                            shadowSnapshot2 = (ISnapshot)new Snapshot<string>(dataReader.IsDBNull(21) ? default(string) : dataReader.GetString(21));
                            return
                            {
                                MountPoint instance;
                                instance = new MountPoint();
                                instance.<Id>k__BackingField = dataReader.IsDBNull(7) ? default(int) : dataReader.GetInt32(7);
                                instance.<BytesPerSecond>k__BackingField = dataReader.IsDBNull(8) ? default(int?) : (int?)dataReader.GetInt32(8);
                                instance.<CreatedAt>k__BackingField = dataReader.IsDBNull(9) ? default(DateTime) : dataReader.GetDateTime(9);
                                instance.<Description>k__BackingField = dataReader.IsDBNull(10) ? default(string) : dataReader.GetString(10);
                                instance.<DetectedFormat>k__BackingField = dataReader.IsDBNull(11) ? default(string) : dataReader.GetString(11);
                                instance.<DetectedNavSystems>k__BackingField = dataReader.IsDBNull(12) ? default(string) : dataReader.GetString(12);
                                instance.<Format>k__BackingField = dataReader.IsDBNull(13) ? default(string) : dataReader.GetString(13);
                                instance.<IsActive>k__BackingField = dataReader.IsDBNull(14) ? default(bool) : dataReader.GetBoolean(14);
                                instance.<LastRtcmMessageTime>k__BackingField = dataReader.IsDBNull(15) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(15);
                                instance.<Latitude>k__BackingField = dataReader.IsDBNull(16) ? default(decimal?) : (decimal?)dataReader.GetDecimal(16);
                                instance.<Longitude>k__BackingField = dataReader.IsDBNull(17) ? default(decimal?) : (decimal?)dataReader.GetDecimal(17);
                                instance.<MaxClients>k__BackingField = dataReader.IsDBNull(18) ? default(int) : dataReader.GetInt32(18);
                                instance.<MessageCount>k__BackingField = dataReader.IsDBNull(19) ? default(int) : dataReader.GetInt32(19);
                                instance.<Name>k__BackingField = dataReader.IsDBNull(20) ? default(string) : dataReader.GetString(20);
                                instance.<ReferenceStationId>k__BackingField = dataReader.IsDBNull(22) ? default(int?) : (int?)dataReader.GetInt32(22);
                                instance.<RequireClientAuthentication>k__BackingField = dataReader.IsDBNull(23) ? default(bool) : dataReader.GetBoolean(23);
                                instance.<RtcmLatitude>k__BackingField = dataReader.IsDBNull(24) ? default(decimal?) : (decimal?)dataReader.GetDecimal(24);
                                instance.<RtcmLongitude>k__BackingField = dataReader.IsDBNull(25) ? default(decimal?) : (decimal?)dataReader.GetDecimal(25);
                                instance.<SourcePassword>k__BackingField = dataReader.IsDBNull(26) ? default(string) : dataReader.GetString(26);
                                instance.<UserId>k__BackingField = dataReader.IsDBNull(27) ? default(string) : dataReader.GetString(27);
                                (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                    context: materializationContext2.Context,
                                    entity: instance,
                                    queryTrackingBehavior: TrackAll,
                                    structuralType: [LIFTABLE Constant: EntityType: MountPoint | Resolver: namelessParameter{6} => namelessParameter{6}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint")]) : default(void);
                                return instance;
                            }}
                    default:
                        default(MountPoint)
                }
                ;
                entry2 = entityType2 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                    entityType: entityType2,
                    entity: instance2,
                    snapshot: shadowSnapshot2);
                return instance2;
            } : default(void);
            return instance2;
        };
        entity =
        {
            MaterializationContext materializationContext3;
            IEntityType entityType3;
            NtripUser instance3;
            InternalEntityEntry entry3;
            bool hasNullKey3;
            materializationContext3 = new MaterializationContext(
                [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                queryContext.Context
            );
            instance3 = default(NtripUser);
            entry3 = queryContext.TryGetEntry(
                key: [LIFTABLE Constant: Key: NtripUser.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser").FindPrimaryKey()],
                keyValues: new object[]{ dataReader.IsDBNull(28) ? default(object) : (object)dataReader.GetString(28) },
                throwOnNullKey: False,
                hasNullKey: hasNullKey3);
            !(hasNullKey3) ? entry3 != default(InternalEntityEntry) ?
            {
                entityType3 = entry3.EntityType;
                return instance3 = (NtripUser)entry3.Entity;
            } :
            {
                ISnapshot shadowSnapshot3;
                shadowSnapshot3 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                entityType3 = [LIFTABLE Constant: EntityType: NtripUser | Resolver: namelessParameter{7} => namelessParameter{7}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser")];
                instance3 = switch (entityType3)
                {
                    case [LIFTABLE Constant: EntityType: NtripUser | Resolver: namelessParameter{8} => namelessParameter{8}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser")]:
                        {
                            return
                            {
                                NtripUser instance;
                                instance = new NtripUser();
                                instance.<Id>k__BackingField = dataReader.IsDBNull(28) ? default(string) : dataReader.GetString(28);
                                instance.<AccessFailedCount>k__BackingField = dataReader.IsDBNull(29) ? default(int) : dataReader.GetInt32(29);
                                instance.<ConcurrencyStamp>k__BackingField = dataReader.IsDBNull(30) ? default(string) : dataReader.GetString(30);
                                instance.<CreatedAt>k__BackingField = dataReader.IsDBNull(31) ? default(DateTime) : dataReader.GetDateTime(31);
                                instance.<Email>k__BackingField = dataReader.IsDBNull(32) ? default(string) : dataReader.GetString(32);
                                instance.<EmailConfirmed>k__BackingField = dataReader.IsDBNull(33) ? default(bool) : dataReader.GetBoolean(33);
                                instance.<FullName>k__BackingField = dataReader.IsDBNull(34) ? default(string) : dataReader.GetString(34);
                                instance.<IsActive>k__BackingField = dataReader.IsDBNull(35) ? default(bool) : dataReader.GetBoolean(35);
                                instance.<LastGeneratedSourcePassword>k__BackingField = dataReader.IsDBNull(36) ? default(string) : dataReader.GetString(36);
                                instance.<LockoutEnabled>k__BackingField = dataReader.IsDBNull(37) ? default(bool) : dataReader.GetBoolean(37);
                                instance.<LockoutEnd>k__BackingField = dataReader.IsDBNull(38) ? default(DateTimeOffset?) : (DateTimeOffset?)dataReader.GetFieldValue<DateTimeOffset>(38);
                                instance.<MaxConnections>k__BackingField = dataReader.IsDBNull(39) ? default(int) : dataReader.GetInt32(39);
                                instance.<NormalizedEmail>k__BackingField = dataReader.IsDBNull(40) ? default(string) : dataReader.GetString(40);
                                instance.<NormalizedUserName>k__BackingField = dataReader.IsDBNull(41) ? default(string) : dataReader.GetString(41);
                                instance.<PasswordHash>k__BackingField = dataReader.IsDBNull(42) ? default(string) : dataReader.GetString(42);
                                instance.<PhoneNumber>k__BackingField = dataReader.IsDBNull(43) ? default(string) : dataReader.GetString(43);
                                instance.<PhoneNumberConfirmed>k__BackingField = dataReader.IsDBNull(44) ? default(bool) : dataReader.GetBoolean(44);
                                instance.<RefreshToken>k__BackingField = dataReader.IsDBNull(45) ? default(string) : dataReader.GetString(45);
                                instance.<RefreshTokenExpires>k__BackingField = dataReader.IsDBNull(46) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(46);
                                instance.<SecurityStamp>k__BackingField = dataReader.IsDBNull(47) ? default(string) : dataReader.GetString(47);
                                instance.<SourcePassword>k__BackingField = dataReader.IsDBNull(48) ? default(string) : dataReader.GetString(48);
                                instance.<SourcePasswordGeneratedAt>k__BackingField = dataReader.IsDBNull(49) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(49);
                                instance.<TwoFactorEnabled>k__BackingField = dataReader.IsDBNull(50) ? default(bool) : dataReader.GetBoolean(50);
                                instance.<UserName>k__BackingField = dataReader.IsDBNull(51) ? default(string) : dataReader.GetString(51);
                                (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                    context: materializationContext3.Context,
                                    entity: instance,
                                    queryTrackingBehavior: TrackAll,
                                    structuralType: [LIFTABLE Constant: EntityType: NtripUser | Resolver: namelessParameter{9} => namelessParameter{9}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser")]) : default(void);
                                return instance;
                            }}
                    default:
                        default(NtripUser)
                }
                ;
                entry3 = entityType3 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                    entityType: entityType3,
                    entity: instance3,
                    snapshot: shadowSnapshot3);
                return instance3;
            } : default(void);
            return instance3;
        };
        ShaperProcessingExpressionVisitor.IncludeReference<Activity, Activity, MountPoint>(
            queryContext: queryContext,
            entity: entity,
            relatedEntity: entity,
            navigation: [LIFTABLE Constant: Navigation: Activity.MountPoint (MountPoint) ToPrincipal MountPoint | Resolver: namelessParameter{10} => namelessParameter{10}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.Activity").FindNavigation("MountPoint")],
            inverseNavigation: default(INavigation),
            fixup: (namelessParameter{11}, namelessParameter{12}) =>
            {
                return namelessParameter{11}.<MountPoint>k__BackingField = namelessParameter{12};
            },
            trackingQuery: True);
        ShaperProcessingExpressionVisitor.IncludeReference<Activity, Activity, NtripUser>(
            queryContext: queryContext,
            entity: entity,
            relatedEntity: entity,
            navigation: [LIFTABLE Constant: Navigation: Activity.User (NtripUser) ToPrincipal NtripUser | Resolver: namelessParameter{13} => namelessParameter{13}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.Activity").FindNavigation("User")],
            inverseNavigation: default(INavigation),
            fixup: (namelessParameter{14}, namelessParameter{15}) =>
            {
                return namelessParameter{14}.<User>k__BackingField = namelessParameter{15};
            },
            trackingQuery: True);
        return entity;
    },
    contextType: AgOpenNtripCaster.Server.Data.ApplicationDbContext,
    standAloneStateManager: False,
    detailedErrorsEnabled: False,
    threadSafetyChecksEnabled: True)'
[23:10:41 ERR] ?? STATS: ActiveClients count = 0
[23:10:41 DBG] Compiling query expression:
'DbSet<MountPoint>()
    .Include(m => m.AllowedGroups)
    .Include(m => m.Owner)
    .Skip(__p_0)
    .Take(__p_1)'
[23:10:41 DBG] Compiling query expression:
'DbSet<SourceConnection>()
    .Where(sc => sc.DisconnectedAt == null)
    .Include(sc => sc.MountPoint)'
[23:10:41 DBG] Including navigation: 'MountPoint.AllowedGroups'.
[23:10:41 DBG] Creating DbConnection.
[23:10:41 DBG] Including navigation: 'SourceConnection.MountPoint'.
[23:10:41 DBG] Including navigation: 'MountPoint.Owner'.
[23:10:41 DBG] Created DbConnection. (4ms).
[23:10:41 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:41 WRN] The query uses a row limiting operator ('Skip'/'Take') without an 'OrderBy' operator. This may lead to unpredictable results. If the 'Distinct' operator is used after 'OrderBy', then make sure to use the 'OrderBy' operator after 'Distinct' as the ordering would otherwise get erased.
[23:10:41 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:41 WRN] The query uses a row limiting operator ('Skip'/'Take') without an 'OrderBy' operator. This may lead to unpredictable results. If the 'Distinct' operator is used after 'OrderBy', then make sure to use the 'OrderBy' operator after 'Distinct' as the ordering would otherwise get erased.
[23:10:41 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:41 DBG] Created DbCommand for 'ExecuteReader' (11ms).
[23:10:41 DBG] Generated query execution expression:
'queryContext => SingleQueryingEnumerable.Create<SourceConnection>(
    relationalQueryContext: (RelationalQueryContext)queryContext,
    relationalCommandResolver: parameters => [LIFTABLE Constant: RelationalCommandCache.QueryExpression(
        Client Projections:
            0 -> Dictionary<IProperty, int> { [Property: SourceConnection.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 0], [Property: SourceConnection.BytesReceived (long) Required, 1], [Property: SourceConnection.BytesSent (long) Required, 2], [Property: SourceConnection.ConnectedAt (DateTime) Required, 3], [Property: SourceConnection.DisconnectedAt (DateTime?), 4], [Property: SourceConnection.MountPointId (int) Required FK Index, 5], [Property: SourceConnection.Status (SourceConnectionStatus) Required, 6] }
            1 -> Dictionary<IProperty, int> { [Property: MountPoint.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 7], [Property: MountPoint.BytesPerSecond (int?), 8], [Property: MountPoint.CreatedAt (DateTime) Required, 9], [Property: MountPoint.Description (string) Required, 10], [Property: MountPoint.DetectedFormat (string), 11], [Property: MountPoint.DetectedNavSystems (string), 12], [Property: MountPoint.Format (string) Required, 13], [Property: MountPoint.IsActive (bool) Required, 14], [Property: MountPoint.LastRtcmMessageTime (DateTime?), 15], [Property: MountPoint.Latitude (decimal?), 16], [Property: MountPoint.Longitude (decimal?), 17], [Property: MountPoint.MaxClients (int) Required, 18], [Property: MountPoint.MessageCount (int) Required, 19], [Property: MountPoint.Name (string) Required, 20], [Property: MountPoint.OwnerId (no field, string) Shadow FK Index, 21], [Property: MountPoint.ReferenceStationId (int?), 22], [Property: MountPoint.RequireClientAuthentication (bool) Required, 23], [Property: MountPoint.RtcmLatitude (decimal?), 24], [Property: MountPoint.RtcmLongitude (decimal?), 25], [Property: MountPoint.SourcePassword (string) Required, 26], [Property: MountPoint.UserId (string), 27] }
        SELECT s.Id, s.BytesReceived, s.BytesSent, s.ConnectedAt, s.DisconnectedAt, s.MountPointId, s.Status, m.Id, m.BytesPerSecond, m.CreatedAt, m.Description, m.DetectedFormat, m.DetectedNavSystems, m.Format, m.IsActive, m.LastRtcmMessageTime, m.Latitude, m.Longitude, m.MaxClients, m.MessageCount, m.Name, m.OwnerId, m.ReferenceStationId, m.RequireClientAuthentication, m.RtcmLatitude, m.RtcmLongitude, m.SourcePassword, m.UserId
        FROM SourceConnections AS s
        INNER JOIN MountPoints AS m ON s.MountPointId == m.Id
        WHERE s.DisconnectedAt == NULL) | Resolver: c => new RelationalCommandCache(
        c.Dependencies.MemoryCache,
        c.RelationalDependencies.QuerySqlGeneratorFactory,
        c.RelationalDependencies.RelationalParameterBasedSqlProcessorFactory,
        Client Projections:
            0 -> Dictionary<IProperty, int> { [Property: SourceConnection.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 0], [Property: SourceConnection.BytesReceived (long) Required, 1], [Property: SourceConnection.BytesSent (long) Required, 2], [Property: SourceConnection.ConnectedAt (DateTime) Required, 3], [Property: SourceConnection.DisconnectedAt (DateTime?), 4], [Property: SourceConnection.MountPointId (int) Required FK Index, 5], [Property: SourceConnection.Status (SourceConnectionStatus) Required, 6] }
            1 -> Dictionary<IProperty, int> { [Property: MountPoint.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 7], [Property: MountPoint.BytesPerSecond (int?), 8], [Property: MountPoint.CreatedAt (DateTime) Required, 9], [Property: MountPoint.Description (string) Required, 10], [Property: MountPoint.DetectedFormat (string), 11], [Property: MountPoint.DetectedNavSystems (string), 12], [Property: MountPoint.Format (string) Required, 13], [Property: MountPoint.IsActive (bool) Required, 14], [Property: MountPoint.LastRtcmMessageTime (DateTime?), 15], [Property: MountPoint.Latitude (decimal?), 16], [Property: MountPoint.Longitude (decimal?), 17], [Property: MountPoint.MaxClients (int) Required, 18], [Property: MountPoint.MessageCount (int) Required, 19], [Property: MountPoint.Name (string) Required, 20], [Property: MountPoint.OwnerId (no field, string) Shadow FK Index, 21], [Property: MountPoint.ReferenceStationId (int?), 22], [Property: MountPoint.RequireClientAuthentication (bool) Required, 23], [Property: MountPoint.RtcmLatitude (decimal?), 24], [Property: MountPoint.RtcmLongitude (decimal?), 25], [Property: MountPoint.SourcePassword (string) Required, 26], [Property: MountPoint.UserId (string), 27] }
        SELECT s.Id, s.BytesReceived, s.BytesSent, s.ConnectedAt, s.DisconnectedAt, s.MountPointId, s.Status, m.Id, m.BytesPerSecond, m.CreatedAt, m.Description, m.DetectedFormat, m.DetectedNavSystems, m.Format, m.IsActive, m.LastRtcmMessageTime, m.Latitude, m.Longitude, m.MaxClients, m.MessageCount, m.Name, m.OwnerId, m.ReferenceStationId, m.RequireClientAuthentication, m.RtcmLatitude, m.RtcmLongitude, m.SourcePassword, m.UserId
        FROM SourceConnections AS s
        INNER JOIN MountPoints AS m ON s.MountPointId == m.Id
        WHERE s.DisconnectedAt == NULL,
        False,
        new HashSet<string>(
            new string[]{ },
            StringComparer.Ordinal
        )
    )].GetRelationalCommandTemplate(parameters),
    readerColumns: null,
    shaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
    {
        SourceConnection entity;
        MountPoint entity;
        entity =
        {
            MaterializationContext materializationContext1;
            IEntityType entityType1;
            SourceConnection instance1;
            InternalEntityEntry entry1;
            bool hasNullKey1;
            materializationContext1 = new MaterializationContext(
                [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                queryContext.Context
            );
            instance1 = default(SourceConnection);
            entry1 = queryContext.TryGetEntry(
                key: [LIFTABLE Constant: Key: SourceConnection.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection").FindPrimaryKey()],
                keyValues: new object[]{ (object)dataReader.GetInt32(0) },
                throwOnNullKey: True,
                hasNullKey: hasNullKey1);
            !(hasNullKey1) ? entry1 != default(InternalEntityEntry) ?
            {
                entityType1 = entry1.EntityType;
                return instance1 = (SourceConnection)entry1.Entity;
            } :
            {
                ISnapshot shadowSnapshot1;
                shadowSnapshot1 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                entityType1 = [LIFTABLE Constant: EntityType: SourceConnection | Resolver: namelessParameter{0} => namelessParameter{0}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection")];
                instance1 = switch (entityType1)
                {
                    case [LIFTABLE Constant: EntityType: SourceConnection | Resolver: namelessParameter{1} => namelessParameter{1}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection")]:
                        {
                            return
                            {
                                SourceConnection instance;
                                instance = new SourceConnection();
                                instance.<Id>k__BackingField = dataReader.GetInt32(0);
                                instance.<BytesReceived>k__BackingField = dataReader.GetInt64(1);
                                instance.<BytesSent>k__BackingField = dataReader.GetInt64(2);
                                instance.<ConnectedAt>k__BackingField = dataReader.GetDateTime(3);
                                instance.<DisconnectedAt>k__BackingField = dataReader.IsDBNull(4) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(4);
                                instance.<MountPointId>k__BackingField = dataReader.GetInt32(5);
                                instance.<Status>k__BackingField = Invoke(((EnumToNumberConverter<SourceConnectionStatus, int>)((IReadOnlyProperty)[LIFTABLE Constant: Property: SourceConnection.Status (SourceConnectionStatus) Required | Resolver: namelessParameter{2} => namelessParameter{2}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection").FindProperty("Status")]).GetTypeMapping().Converter).ConvertFromProviderTyped, dataReader.GetInt32(6));
                                (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                    context: materializationContext1.Context,
                                    entity: instance,
                                    queryTrackingBehavior: TrackAll,
                                    structuralType: [LIFTABLE Constant: EntityType: SourceConnection | Resolver: namelessParameter{3} => namelessParameter{3}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection")]) : default(void);
                                return instance;
                            }}
                    default:
                        default(SourceConnection)
                }
                ;
                entry1 = entityType1 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                    entityType: entityType1,
                    entity: instance1,
                    snapshot: shadowSnapshot1);
                return instance1;
            } : default(void);
            return instance1;
        };
        entity =
        {
            MaterializationContext materializationContext2;
            IEntityType entityType2;
            MountPoint instance2;
            InternalEntityEntry entry2;
            bool hasNullKey2;
            materializationContext2 = new MaterializationContext(
                [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                queryContext.Context
            );
            instance2 = default(MountPoint);
            entry2 = queryContext.TryGetEntry(
                key: [LIFTABLE Constant: Key: MountPoint.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint").FindPrimaryKey()],
                keyValues: new object[]{ (object)dataReader.GetInt32(7) },
                throwOnNullKey: True,
                hasNullKey: hasNullKey2);
            !(hasNullKey2) ? entry2 != default(InternalEntityEntry) ?
            {
                entityType2 = entry2.EntityType;
                return instance2 = (MountPoint)entry2.Entity;
            } :
            {
                ISnapshot shadowSnapshot2;
                shadowSnapshot2 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                entityType2 = [LIFTABLE Constant: EntityType: MountPoint | Resolver: namelessParameter{4} => namelessParameter{4}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint")];
                instance2 = switch (entityType2)
                {
                    case [LIFTABLE Constant: EntityType: MountPoint | Resolver: namelessParameter{5} => namelessParameter{5}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint")]:
                        {
                            shadowSnapshot2 = (ISnapshot)new Snapshot<string>(dataReader.IsDBNull(21) ? default(string) : dataReader.GetString(21));
                            return
                            {
                                MountPoint instance;
                                instance = new MountPoint();
                                instance.<Id>k__BackingField = dataReader.GetInt32(7);
                                instance.<BytesPerSecond>k__BackingField = dataReader.IsDBNull(8) ? default(int?) : (int?)dataReader.GetInt32(8);
                                instance.<CreatedAt>k__BackingField = dataReader.GetDateTime(9);
                                instance.<Description>k__BackingField = dataReader.GetString(10);
                                instance.<DetectedFormat>k__BackingField = dataReader.IsDBNull(11) ? default(string) : dataReader.GetString(11);
                                instance.<DetectedNavSystems>k__BackingField = dataReader.IsDBNull(12) ? default(string) : dataReader.GetString(12);
                                instance.<Format>k__BackingField = dataReader.GetString(13);
                                instance.<IsActive>k__BackingField = dataReader.GetBoolean(14);
                                instance.<LastRtcmMessageTime>k__BackingField = dataReader.IsDBNull(15) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(15);
                                instance.<Latitude>k__BackingField = dataReader.IsDBNull(16) ? default(decimal?) : (decimal?)dataReader.GetDecimal(16);
                                instance.<Longitude>k__BackingField = dataReader.IsDBNull(17) ? default(decimal?) : (decimal?)dataReader.GetDecimal(17);
                                instance.<MaxClients>k__BackingField = dataReader.GetInt32(18);
                                instance.<MessageCount>k__BackingField = dataReader.GetInt32(19);
                                instance.<Name>k__BackingField = dataReader.GetString(20);
                                instance.<ReferenceStationId>k__BackingField = dataReader.IsDBNull(22) ? default(int?) : (int?)dataReader.GetInt32(22);
                                instance.<RequireClientAuthentication>k__BackingField = dataReader.GetBoolean(23);
                                instance.<RtcmLatitude>k__BackingField = dataReader.IsDBNull(24) ? default(decimal?) : (decimal?)dataReader.GetDecimal(24);
                                instance.<RtcmLongitude>k__BackingField = dataReader.IsDBNull(25) ? default(decimal?) : (decimal?)dataReader.GetDecimal(25);
                                instance.<SourcePassword>k__BackingField = dataReader.GetString(26);
                                instance.<UserId>k__BackingField = dataReader.IsDBNull(27) ? default(string) : dataReader.GetString(27);
                                (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                    context: materializationContext2.Context,
                                    entity: instance,
                                    queryTrackingBehavior: TrackAll,
                                    structuralType: [LIFTABLE Constant: EntityType: MountPoint | Resolver: namelessParameter{6} => namelessParameter{6}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint")]) : default(void);
                                return instance;
                            }}
                    default:
                        default(MountPoint)
                }
                ;
                entry2 = entityType2 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                    entityType: entityType2,
                    entity: instance2,
                    snapshot: shadowSnapshot2);
                return instance2;
            } : default(void);
            return instance2;
        };
        ShaperProcessingExpressionVisitor.IncludeReference<SourceConnection, SourceConnection, MountPoint>(
            queryContext: queryContext,
            entity: entity,
            relatedEntity: entity,
            navigation: [LIFTABLE Constant: Navigation: SourceConnection.MountPoint (MountPoint) ToPrincipal MountPoint Inverse: SourceConnections | Resolver: namelessParameter{7} => namelessParameter{7}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.SourceConnection").FindNavigation("MountPoint")],
            inverseNavigation: [LIFTABLE Constant: Navigation: MountPoint.SourceConnections (ICollection<SourceConnection>) Collection ToDependent SourceConnection Inverse: MountPoint | Resolver: namelessParameter{8} => namelessParameter{8}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint").FindNavigation("SourceConnections")],
            fixup: (namelessParameter{9}, namelessParameter{10}) =>
            {
                namelessParameter{9}.<MountPoint>k__BackingField = namelessParameter{10};
                return [LIFTABLE Constant: ClrICollectionAccessor<MountPoint, ICollection<SourceConnection>, SourceConnection> | Resolver: namelessParameter{11} => namelessParameter{11}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint").FindNavigation("SourceConnections").GetCollectionAccessor()].Add(
                    entity: namelessParameter{10},
                    value: namelessParameter{9},
                    forMaterialization: True);
            },
            trackingQuery: True);
        return entity;
    },
    contextType: AgOpenNtripCaster.Server.Data.ApplicationDbContext,
    standAloneStateManager: False,
    detailedErrorsEnabled: False,
    threadSafetyChecksEnabled: True)'
[23:10:41 DBG] Initialized DbCommand for 'ExecuteReader' (14ms).
[23:10:41 DBG] Generated query execution expression:
'queryContext => SingleQueryingEnumerable.Create<MountPoint>(
    relationalQueryContext: (RelationalQueryContext)queryContext,
    relationalCommandResolver: parameters => [LIFTABLE Constant: RelationalCommandCache.QueryExpression(
        Client Projections:
            0 -> Dictionary<IProperty, int> { [Property: MountPoint.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 0], [Property: MountPoint.BytesPerSecond (int?), 1], [Property: MountPoint.CreatedAt (DateTime) Required, 2], [Property: MountPoint.Description (string) Required, 3], [Property: MountPoint.DetectedFormat (string), 4], [Property: MountPoint.DetectedNavSystems (string), 5], [Property: MountPoint.Format (string) Required, 6], [Property: MountPoint.IsActive (bool) Required, 7], [Property: MountPoint.LastRtcmMessageTime (DateTime?), 8], [Property: MountPoint.Latitude (decimal?), 9], [Property: MountPoint.Longitude (decimal?), 10], [Property: MountPoint.MaxClients (int) Required, 11], [Property: MountPoint.MessageCount (int) Required, 12], [Property: MountPoint.Name (string) Required, 13], [Property: MountPoint.OwnerId (no field, string) Shadow FK Index, 14], [Property: MountPoint.ReferenceStationId (int?), 15], [Property: MountPoint.RequireClientAuthentication (bool) Required, 16], [Property: MountPoint.RtcmLatitude (decimal?), 17], [Property: MountPoint.RtcmLongitude (decimal?), 18], [Property: MountPoint.SourcePassword (string) Required, 19], [Property: MountPoint.UserId (string), 20] }
            1 -> 0
            2 -> 21
            3 -> Dictionary<IProperty, int> { [Property: MountPointNtripGroup (Dictionary<string, object>).AllowedGroupsId (no field, int) Indexer Required PK FK AfterSave:Throw, 22], [Property: MountPointNtripGroup (Dictionary<string, object>).MountPointsId (no field, int) Indexer Required PK FK Index AfterSave:Throw, 23] }
            4 -> Dictionary<IProperty, int> { [Property: NtripGroup.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 24], [Property: NtripGroup.CreatedAt (DateTime) Required, 25], [Property: NtripGroup.Description (string) Required, 26], [Property: NtripGroup.IsActive (bool) Required, 27], [Property: NtripGroup.Name (string) Required, 28] }
            5 -> 22
            6 -> 23
            7 -> 24
            8 -> Dictionary<IProperty, int> { [Property: NtripUser.Id (string) Required PK AfterSave:Throw, 21], [Property: NtripUser.AccessFailedCount (int) Required, 29], [Property: NtripUser.ConcurrencyStamp (string) Concurrency, 30], [Property: NtripUser.CreatedAt (DateTime) Required, 31], [Property: NtripUser.Email (string) MaxLength(256), 32], [Property: NtripUser.EmailConfirmed (bool) Required, 33], [Property: NtripUser.FullName (string) Required, 34], [Property: NtripUser.IsActive (bool) Required, 35], [Property: NtripUser.LastGeneratedSourcePassword (string), 36], [Property: NtripUser.LockoutEnabled (bool) Required, 37], [Property: NtripUser.LockoutEnd (DateTimeOffset?), 38], [Property: NtripUser.MaxConnections (int) Required, 39], [Property: NtripUser.NormalizedEmail (string) Index MaxLength(256), 40], [Property: NtripUser.NormalizedUserName (string) Index MaxLength(256), 41], [Property: NtripUser.PasswordHash (string), 42], [Property: NtripUser.PhoneNumber (string), 43], [Property: NtripUser.PhoneNumberConfirmed (bool) Required, 44], [Property: NtripUser.RefreshToken (string), 45], [Property: NtripUser.RefreshTokenExpires (DateTime?), 46], [Property: NtripUser.SecurityStamp (string), 47], [Property: NtripUser.SourcePassword (string), 48], [Property: NtripUser.SourcePasswordGeneratedAt (DateTime?), 49], [Property: NtripUser.TwoFactorEnabled (bool) Required, 50], [Property: NtripUser.UserName (string) MaxLength(256), 51] }
        SELECT m0.Id, m0.BytesPerSecond, m0.CreatedAt, m0.Description, m0.DetectedFormat, m0.DetectedNavSystems, m0.Format, m0.IsActive, m0.LastRtcmMessageTime, m0.Latitude, m0.Longitude, m0.MaxClients, m0.MessageCount, m0.Name, m0.OwnerId, m0.ReferenceStationId, m0.RequireClientAuthentication, m0.RtcmLatitude, m0.RtcmLongitude, m0.SourcePassword, m0.UserId, a.Id, s.AllowedGroupsId, s.MountPointsId, s.Id, s.CreatedAt, s.Description, s.IsActive, s.Name, a.AccessFailedCount, a.ConcurrencyStamp, a.CreatedAt, a.Email, a.EmailConfirmed, a.FullName, a.IsActive, a.LastGeneratedSourcePassword, a.LockoutEnabled, a.LockoutEnd, a.MaxConnections, a.NormalizedEmail, a.NormalizedUserName, a.PasswordHash, a.PhoneNumber, a.PhoneNumberConfirmed, a.RefreshToken, a.RefreshTokenExpires, a.SecurityStamp, a.SourcePassword, a.SourcePasswordGeneratedAt, a.TwoFactorEnabled, a.UserName
        FROM
        (
            SELECT m.Id, m.BytesPerSecond, m.CreatedAt, m.Description, m.DetectedFormat, m.DetectedNavSystems, m.Format, m.IsActive, m.LastRtcmMessageTime, m.Latitude, m.Longitude, m.MaxClients, m.MessageCount, m.Name, m.OwnerId, m.ReferenceStationId, m.RequireClientAuthentication, m.RtcmLatitude, m.RtcmLongitude, m.SourcePassword, m.UserId
            FROM MountPoints AS m
            OFFSET @__p_0 ROWS FETCH NEXT @__p_1 ROWS ONLY
        ) AS m0
        LEFT JOIN AspNetUsers AS a ON m0.OwnerId == a.Id
        LEFT JOIN
        (
            SELECT g.AllowedGroupsId, g.MountPointsId, n.Id, n.CreatedAt, n.Description, n.IsActive, n.Name
            FROM GroupMountPoints AS g
            INNER JOIN NtripGroups AS n ON g.AllowedGroupsId == n.Id
        ) AS s ON m0.Id == s.MountPointsId
        ORDER BY m0.Id ASC, a.Id ASC, s.AllowedGroupsId ASC, s.MountPointsId ASC) | Resolver: c => new RelationalCommandCache(
        c.Dependencies.MemoryCache,
        c.RelationalDependencies.QuerySqlGeneratorFactory,
        c.RelationalDependencies.RelationalParameterBasedSqlProcessorFactory,
        Client Projections:
            0 -> Dictionary<IProperty, int> { [Property: MountPoint.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 0], [Property: MountPoint.BytesPerSecond (int?), 1], [Property: MountPoint.CreatedAt (DateTime) Required, 2], [Property: MountPoint.Description (string) Required, 3], [Property: MountPoint.DetectedFormat (string), 4], [Property: MountPoint.DetectedNavSystems (string), 5], [Property: MountPoint.Format (string) Required, 6], [Property: MountPoint.IsActive (bool) Required, 7], [Property: MountPoint.LastRtcmMessageTime (DateTime?), 8], [Property: MountPoint.Latitude (decimal?), 9], [Property: MountPoint.Longitude (decimal?), 10], [Property: MountPoint.MaxClients (int) Required, 11], [Property: MountPoint.MessageCount (int) Required, 12], [Property: MountPoint.Name (string) Required, 13], [Property: MountPoint.OwnerId (no field, string) Shadow FK Index, 14], [Property: MountPoint.ReferenceStationId (int?), 15], [Property: MountPoint.RequireClientAuthentication (bool) Required, 16], [Property: MountPoint.RtcmLatitude (decimal?), 17], [Property: MountPoint.RtcmLongitude (decimal?), 18], [Property: MountPoint.SourcePassword (string) Required, 19], [Property: MountPoint.UserId (string), 20] }
            1 -> 0
            2 -> 21
            3 -> Dictionary<IProperty, int> { [Property: MountPointNtripGroup (Dictionary<string, object>).AllowedGroupsId (no field, int) Indexer Required PK FK AfterSave:Throw, 22], [Property: MountPointNtripGroup (Dictionary<string, object>).MountPointsId (no field, int) Indexer Required PK FK Index AfterSave:Throw, 23] }
            4 -> Dictionary<IProperty, int> { [Property: NtripGroup.Id (int) Required PK AfterSave:Throw ValueGenerated.OnAdd, 24], [Property: NtripGroup.CreatedAt (DateTime) Required, 25], [Property: NtripGroup.Description (string) Required, 26], [Property: NtripGroup.IsActive (bool) Required, 27], [Property: NtripGroup.Name (string) Required, 28] }
            5 -> 22
            6 -> 23
            7 -> 24
            8 -> Dictionary<IProperty, int> { [Property: NtripUser.Id (string) Required PK AfterSave:Throw, 21], [Property: NtripUser.AccessFailedCount (int) Required, 29], [Property: NtripUser.ConcurrencyStamp (string) Concurrency, 30], [Property: NtripUser.CreatedAt (DateTime) Required, 31], [Property: NtripUser.Email (string) MaxLength(256), 32], [Property: NtripUser.EmailConfirmed (bool) Required, 33], [Property: NtripUser.FullName (string) Required, 34], [Property: NtripUser.IsActive (bool) Required, 35], [Property: NtripUser.LastGeneratedSourcePassword (string), 36], [Property: NtripUser.LockoutEnabled (bool) Required, 37], [Property: NtripUser.LockoutEnd (DateTimeOffset?), 38], [Property: NtripUser.MaxConnections (int) Required, 39], [Property: NtripUser.NormalizedEmail (string) Index MaxLength(256), 40], [Property: NtripUser.NormalizedUserName (string) Index MaxLength(256), 41], [Property: NtripUser.PasswordHash (string), 42], [Property: NtripUser.PhoneNumber (string), 43], [Property: NtripUser.PhoneNumberConfirmed (bool) Required, 44], [Property: NtripUser.RefreshToken (string), 45], [Property: NtripUser.RefreshTokenExpires (DateTime?), 46], [Property: NtripUser.SecurityStamp (string), 47], [Property: NtripUser.SourcePassword (string), 48], [Property: NtripUser.SourcePasswordGeneratedAt (DateTime?), 49], [Property: NtripUser.TwoFactorEnabled (bool) Required, 50], [Property: NtripUser.UserName (string) MaxLength(256), 51] }
        SELECT m0.Id, m0.BytesPerSecond, m0.CreatedAt, m0.Description, m0.DetectedFormat, m0.DetectedNavSystems, m0.Format, m0.IsActive, m0.LastRtcmMessageTime, m0.Latitude, m0.Longitude, m0.MaxClients, m0.MessageCount, m0.Name, m0.OwnerId, m0.ReferenceStationId, m0.RequireClientAuthentication, m0.RtcmLatitude, m0.RtcmLongitude, m0.SourcePassword, m0.UserId, a.Id, s.AllowedGroupsId, s.MountPointsId, s.Id, s.CreatedAt, s.Description, s.IsActive, s.Name, a.AccessFailedCount, a.ConcurrencyStamp, a.CreatedAt, a.Email, a.EmailConfirmed, a.FullName, a.IsActive, a.LastGeneratedSourcePassword, a.LockoutEnabled, a.LockoutEnd, a.MaxConnections, a.NormalizedEmail, a.NormalizedUserName, a.PasswordHash, a.PhoneNumber, a.PhoneNumberConfirmed, a.RefreshToken, a.RefreshTokenExpires, a.SecurityStamp, a.SourcePassword, a.SourcePasswordGeneratedAt, a.TwoFactorEnabled, a.UserName
        FROM
        (
            SELECT m.Id, m.BytesPerSecond, m.CreatedAt, m.Description, m.DetectedFormat, m.DetectedNavSystems, m.Format, m.IsActive, m.LastRtcmMessageTime, m.Latitude, m.Longitude, m.MaxClients, m.MessageCount, m.Name, m.OwnerId, m.ReferenceStationId, m.RequireClientAuthentication, m.RtcmLatitude, m.RtcmLongitude, m.SourcePassword, m.UserId
            FROM MountPoints AS m
            OFFSET @__p_0 ROWS FETCH NEXT @__p_1 ROWS ONLY
        ) AS m0
        LEFT JOIN AspNetUsers AS a ON m0.OwnerId == a.Id
        LEFT JOIN
        (
            SELECT g.AllowedGroupsId, g.MountPointsId, n.Id, n.CreatedAt, n.Description, n.IsActive, n.Name
            FROM GroupMountPoints AS g
            INNER JOIN NtripGroups AS n ON g.AllowedGroupsId == n.Id
        ) AS s ON m0.Id == s.MountPointsId
        ORDER BY m0.Id ASC, a.Id ASC, s.AllowedGroupsId ASC, s.MountPointsId ASC,
        False,
        new HashSet<string>(
            new string[]{ },
            StringComparer.Ordinal
        )
    )].GetRelationalCommandTemplate(parameters),
    readerColumns: null,
    shaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
    {
        resultContext.Values == null ?
        {
            MountPoint entity;
            NtripUser entity;
            entity =
            {
                MaterializationContext materializationContext1;
                IEntityType entityType1;
                MountPoint instance1;
                InternalEntityEntry entry1;
                bool hasNullKey1;
                materializationContext1 = new MaterializationContext(
                    [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                    queryContext.Context
                );
                instance1 = default(MountPoint);
                entry1 = queryContext.TryGetEntry(
                    key: [LIFTABLE Constant: Key: MountPoint.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint").FindPrimaryKey()],
                    keyValues: new object[]{ (object)dataReader.GetInt32(0) },
                    throwOnNullKey: True,
                    hasNullKey: hasNullKey1);
                !(hasNullKey1) ? entry1 != default(InternalEntityEntry) ?
                {
                    entityType1 = entry1.EntityType;
                    return instance1 = (MountPoint)entry1.Entity;
                } :
                {
                    ISnapshot shadowSnapshot1;
                    shadowSnapshot1 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                    entityType1 = [LIFTABLE Constant: EntityType: MountPoint | Resolver: namelessParameter{0} => namelessParameter{0}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint")];
                    instance1 = switch (entityType1)
                    {
                        case [LIFTABLE Constant: EntityType: MountPoint | Resolver: namelessParameter{1} => namelessParameter{1}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint")]:
                            {
                                shadowSnapshot1 = (ISnapshot)new Snapshot<string>(dataReader.IsDBNull(14) ? default(string) : dataReader.GetString(14));
                                return
                                {
                                    MountPoint instance;
                                    instance = new MountPoint();
                                    instance.<Id>k__BackingField = dataReader.GetInt32(0);
                                    instance.<BytesPerSecond>k__BackingField = dataReader.IsDBNull(1) ? default(int?) : (int?)dataReader.GetInt32(1);
                                    instance.<CreatedAt>k__BackingField = dataReader.GetDateTime(2);
                                    instance.<Description>k__BackingField = dataReader.GetString(3);
                                    instance.<DetectedFormat>k__BackingField = dataReader.IsDBNull(4) ? default(string) : dataReader.GetString(4);
                                    instance.<DetectedNavSystems>k__BackingField = dataReader.IsDBNull(5) ? default(string) : dataReader.GetString(5);
                                    instance.<Format>k__BackingField = dataReader.GetString(6);
                                    instance.<IsActive>k__BackingField = dataReader.GetBoolean(7);
                                    instance.<LastRtcmMessageTime>k__BackingField = dataReader.IsDBNull(8) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(8);
                                    instance.<Latitude>k__BackingField = dataReader.IsDBNull(9) ? default(decimal?) : (decimal?)dataReader.GetDecimal(9);
                                    instance.<Longitude>k__BackingField = dataReader.IsDBNull(10) ? default(decimal?) : (decimal?)dataReader.GetDecimal(10);
                                    instance.<MaxClients>k__BackingField = dataReader.GetInt32(11);
                                    instance.<MessageCount>k__BackingField = dataReader.GetInt32(12);
                                    instance.<Name>k__BackingField = dataReader.GetString(13);
                                    instance.<ReferenceStationId>k__BackingField = dataReader.IsDBNull(15) ? default(int?) : (int?)dataReader.GetInt32(15);
                                    instance.<RequireClientAuthentication>k__BackingField = dataReader.GetBoolean(16);
                                    instance.<RtcmLatitude>k__BackingField = dataReader.IsDBNull(17) ? default(decimal?) : (decimal?)dataReader.GetDecimal(17);
                                    instance.<RtcmLongitude>k__BackingField = dataReader.IsDBNull(18) ? default(decimal?) : (decimal?)dataReader.GetDecimal(18);
                                    instance.<SourcePassword>k__BackingField = dataReader.GetString(19);
                                    instance.<UserId>k__BackingField = dataReader.IsDBNull(20) ? default(string) : dataReader.GetString(20);
                                    (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                        context: materializationContext1.Context,
                                        entity: instance,
                                        queryTrackingBehavior: TrackAll,
                                        structuralType: [LIFTABLE Constant: EntityType: MountPoint | Resolver: namelessParameter{2} => namelessParameter{2}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint")]) : default(void);
                                    return instance;
                                }}
                        default:
                            default(MountPoint)
                    }
                    ;
                    entry1 = entityType1 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                        entityType: entityType1,
                        entity: instance1,
                        snapshot: shadowSnapshot1);
                    return instance1;
                } : default(void);
                return instance1;
            };
            entity =
            {
                MaterializationContext materializationContext4;
                IEntityType entityType4;
                NtripUser instance4;
                InternalEntityEntry entry4;
                bool hasNullKey4;
                materializationContext4 = new MaterializationContext(
                    [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                    queryContext.Context
                );
                instance4 = default(NtripUser);
                entry4 = queryContext.TryGetEntry(
                    key: [LIFTABLE Constant: Key: NtripUser.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser").FindPrimaryKey()],
                    keyValues: new object[]{ dataReader.IsDBNull(21) ? default(object) : (object)dataReader.GetString(21) },
                    throwOnNullKey: False,
                    hasNullKey: hasNullKey4);
                !(hasNullKey4) ? entry4 != default(InternalEntityEntry) ?
                {
                    entityType4 = entry4.EntityType;
                    return instance4 = (NtripUser)entry4.Entity;
                } :
                {
                    ISnapshot shadowSnapshot4;
                    shadowSnapshot4 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                    entityType4 = [LIFTABLE Constant: EntityType: NtripUser | Resolver: namelessParameter{3} => namelessParameter{3}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser")];
                    instance4 = switch (entityType4)
                    {
                        case [LIFTABLE Constant: EntityType: NtripUser | Resolver: namelessParameter{4} => namelessParameter{4}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser")]:
                            {
                                return
                                {
                                    NtripUser instance;
                                    instance = new NtripUser();
                                    instance.<Id>k__BackingField = dataReader.IsDBNull(21) ? default(string) : dataReader.GetString(21);
                                    instance.<AccessFailedCount>k__BackingField = dataReader.IsDBNull(29) ? default(int) : dataReader.GetInt32(29);
                                    instance.<ConcurrencyStamp>k__BackingField = dataReader.IsDBNull(30) ? default(string) : dataReader.GetString(30);
                                    instance.<CreatedAt>k__BackingField = dataReader.IsDBNull(31) ? default(DateTime) : dataReader.GetDateTime(31);
                                    instance.<Email>k__BackingField = dataReader.IsDBNull(32) ? default(string) : dataReader.GetString(32);
                                    instance.<EmailConfirmed>k__BackingField = dataReader.IsDBNull(33) ? default(bool) : dataReader.GetBoolean(33);
                                    instance.<FullName>k__BackingField = dataReader.IsDBNull(34) ? default(string) : dataReader.GetString(34);
                                    instance.<IsActive>k__BackingField = dataReader.IsDBNull(35) ? default(bool) : dataReader.GetBoolean(35);
                                    instance.<LastGeneratedSourcePassword>k__BackingField = dataReader.IsDBNull(36) ? default(string) : dataReader.GetString(36);
                                    instance.<LockoutEnabled>k__BackingField = dataReader.IsDBNull(37) ? default(bool) : dataReader.GetBoolean(37);
                                    instance.<LockoutEnd>k__BackingField = dataReader.IsDBNull(38) ? default(DateTimeOffset?) : (DateTimeOffset?)dataReader.GetFieldValue<DateTimeOffset>(38);
                                    instance.<MaxConnections>k__BackingField = dataReader.IsDBNull(39) ? default(int) : dataReader.GetInt32(39);
                                    instance.<NormalizedEmail>k__BackingField = dataReader.IsDBNull(40) ? default(string) : dataReader.GetString(40);
                                    instance.<NormalizedUserName>k__BackingField = dataReader.IsDBNull(41) ? default(string) : dataReader.GetString(41);
                                    instance.<PasswordHash>k__BackingField = dataReader.IsDBNull(42) ? default(string) : dataReader.GetString(42);
                                    instance.<PhoneNumber>k__BackingField = dataReader.IsDBNull(43) ? default(string) : dataReader.GetString(43);
                                    instance.<PhoneNumberConfirmed>k__BackingField = dataReader.IsDBNull(44) ? default(bool) : dataReader.GetBoolean(44);
                                    instance.<RefreshToken>k__BackingField = dataReader.IsDBNull(45) ? default(string) : dataReader.GetString(45);
                                    instance.<RefreshTokenExpires>k__BackingField = dataReader.IsDBNull(46) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(46);
                                    instance.<SecurityStamp>k__BackingField = dataReader.IsDBNull(47) ? default(string) : dataReader.GetString(47);
                                    instance.<SourcePassword>k__BackingField = dataReader.IsDBNull(48) ? default(string) : dataReader.GetString(48);
                                    instance.<SourcePasswordGeneratedAt>k__BackingField = dataReader.IsDBNull(49) ? default(DateTime?) : (DateTime?)dataReader.GetDateTime(49);
                                    instance.<TwoFactorEnabled>k__BackingField = dataReader.IsDBNull(50) ? default(bool) : dataReader.GetBoolean(50);
                                    instance.<UserName>k__BackingField = dataReader.IsDBNull(51) ? default(string) : dataReader.GetString(51);
                                    (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                        context: materializationContext4.Context,
                                        entity: instance,
                                        queryTrackingBehavior: TrackAll,
                                        structuralType: [LIFTABLE Constant: EntityType: NtripUser | Resolver: namelessParameter{5} => namelessParameter{5}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser")]) : default(void);
                                    return instance;
                                }}
                        default:
                            default(NtripUser)
                    }
                    ;
                    entry4 = entityType4 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                        entityType: entityType4,
                        entity: instance4,
                        snapshot: shadowSnapshot4);
                    return instance4;
                } : default(void);
                return instance4;
            };
            resultContext.Values = new object[]
            {
                entity,
                entity
            };
            ShaperProcessingExpressionVisitor.InitializeIncludeCollection<MountPoint, MountPoint>(
                collectionId: 0,
                queryContext: queryContext,
                dbDataReader: dataReader,
                resultCoordinator: resultCoordinator,
                entity: (MountPoint)(resultContext.Values[0]),
                parentIdentifier: [LIFTABLE Constant: Func<QueryContext, DbDataReader, object[]> | Resolver: _ => (queryContext, dataReader) => new object[]
                {
                    (object)(int?)dataReader.GetInt32(0),
                    dataReader.IsDBNull(21) ? default(string) : dataReader.GetString(21)
                }],
                outerIdentifier: [LIFTABLE Constant: Func<QueryContext, DbDataReader, object[]> | Resolver: _ => (queryContext, dataReader) => new object[]
                {
                    (object)(int?)dataReader.GetInt32(0),
                    dataReader.IsDBNull(21) ? default(string) : dataReader.GetString(21)
                }],
                navigation: [LIFTABLE Constant: SkipNavigation: MountPoint.AllowedGroups (ICollection<NtripGroup>) CollectionNtripGroup Inverse: MountPoints | Resolver: namelessParameter{6} => namelessParameter{6}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint").FindSkipNavigation("AllowedGroups")],
                clrCollectionAccessor: [LIFTABLE Constant: ClrICollectionAccessor<MountPoint, ICollection<NtripGroup>, NtripGroup> | Resolver: namelessParameter{7} => namelessParameter{7}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint").FindSkipNavigation("AllowedGroups").GetCollectionAccessor()],
                trackingQuery: True,
                setLoaded: True);
            ShaperProcessingExpressionVisitor.IncludeReference<MountPoint, MountPoint, NtripUser>(
                queryContext: queryContext,
                entity: (MountPoint)(resultContext.Values[0]),
                relatedEntity: (NtripUser)(resultContext.Values[1]),
                navigation: [LIFTABLE Constant: Navigation: MountPoint.Owner (NtripUser) ToPrincipal NtripUser Inverse: OwnedMountPoints | Resolver: namelessParameter{8} => namelessParameter{8}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint").FindNavigation("Owner")],
                inverseNavigation: [LIFTABLE Constant: Navigation: NtripUser.OwnedMountPoints (ICollection<MountPoint>) Collection ToDependent MountPoint Inverse: Owner | Resolver: namelessParameter{9} => namelessParameter{9}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser").FindNavigation("OwnedMountPoints")],
                fixup: (namelessParameter{10}, namelessParameter{11}) =>
                {
                    namelessParameter{10}.<Owner>k__BackingField = namelessParameter{11};
                    return [LIFTABLE Constant: ClrICollectionAccessor<NtripUser, ICollection<MountPoint>, MountPoint> | Resolver: namelessParameter{12} => namelessParameter{12}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripUser").FindNavigation("OwnedMountPoints").GetCollectionAccessor()].Add(
                        entity: namelessParameter{11},
                        value: namelessParameter{10},
                        forMaterialization: True);
                },
                trackingQuery: True);
        } : default(void);
        ShaperProcessingExpressionVisitor.PopulateIncludeCollection<MountPoint, NtripGroup>(
            collectionId: 0,
            queryContext: queryContext,
            dbDataReader: dataReader,
            resultCoordinator: resultCoordinator,
            parentIdentifier: [LIFTABLE Constant: Func<QueryContext, DbDataReader, object[]> | Resolver: _ => (queryContext, dataReader) => new object[]
            {
                (object)(int?)dataReader.GetInt32(0),
                dataReader.IsDBNull(21) ? default(string) : dataReader.GetString(21)
            }],
            outerIdentifier: [LIFTABLE Constant: Func<QueryContext, DbDataReader, object[]> | Resolver: _ => (queryContext, dataReader) => new object[]
            {
                (object)(int?)dataReader.GetInt32(0),
                dataReader.IsDBNull(21) ? default(string) : dataReader.GetString(21)
            }],
            selfIdentifier: [LIFTABLE Constant: Func<QueryContext, DbDataReader, object[]> | Resolver: _ => (queryContext, dataReader) => new object[]
            {
                (object)dataReader.IsDBNull(22) ? default(int?) : (int?)dataReader.GetInt32(22),
                (object)dataReader.IsDBNull(23) ? default(int?) : (int?)dataReader.GetInt32(23),
                (object)dataReader.IsDBNull(24) ? default(int?) : (int?)dataReader.GetInt32(24)
            }],
            parentIdentifierValueComparers: [LIFTABLE Constant: Func<object, object, bool>[] { Func<object, object, bool>, Func<object, object, bool> } | Resolver: _ => new Func<object, object, bool>[]
            {
                (left, right) => left == null ? right == null : right != null && (int)left == (int)right,
                (left, right) => left == null ? right == null : right != null && (string)left == (string)right
            }],
            outerIdentifierValueComparers: [LIFTABLE Constant: Func<object, object, bool>[] { Func<object, object, bool>, Func<object, object, bool> } | Resolver: _ => new Func<object, object, bool>[]
            {
                (left, right) => left == null ? right == null : right != null && (int)left == (int)right,
                (left, right) => left == null ? right == null : right != null && (string)left == (string)right
            }],
            selfIdentifierValueComparers: [LIFTABLE Constant: Func<object, object, bool>[] { Func<object, object, bool>, Func<object, object, bool>, Func<object, object, bool> } | Resolver: _ => new Func<object, object, bool>[]
            {
                (left, right) => left == null ? right == null : right != null && (int)left == (int)right,
                (left, right) => left == null ? right == null : right != null && (int)left == (int)right,
                (left, right) => left == null ? right == null : right != null && (int)left == (int)right
            }],
            innerShaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
            {
                Dictionary<string, object> entity;
                NtripGroup entity;
                entity =
                {
                    MaterializationContext materializationContext2;
                    IEntityType entityType2;
                    Dictionary<string, object> instance2;
                    InternalEntityEntry entry2;
                    bool hasNullKey2;
                    materializationContext2 = new MaterializationContext(
                        [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                        queryContext.Context
                    );
                    instance2 = default(Dictionary<string, object>);
                    entry2 = queryContext.TryGetEntry(
                        key: [LIFTABLE Constant: Key: MountPointNtripGroup (Dictionary<string, object>).AllowedGroupsId, MountPointNtripGroup (Dictionary<string, object>).MountPointsId PK | Resolver: c => c.Dependencies.Model.FindEntityType("MountPointNtripGroup").FindPrimaryKey()],
                        keyValues: new object[]
                        {
                            dataReader.IsDBNull(22) ? default(object) : (object)dataReader.GetInt32(22),
                            dataReader.IsDBNull(23) ? default(object) : (object)dataReader.GetInt32(23)
                        },
                        throwOnNullKey: False,
                        hasNullKey: hasNullKey2);
                    !(hasNullKey2) ? entry2 != default(InternalEntityEntry) ?
                    {
                        entityType2 = entry2.EntityType;
                        return instance2 = (Dictionary<string, object>)entry2.Entity;
                    } :
                    {
                        ISnapshot shadowSnapshot2;
                        shadowSnapshot2 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                        entityType2 = [LIFTABLE Constant: EntityType: MountPointNtripGroup (Dictionary<string, object>) CLR Type: Dictionary<string, object> | Resolver: namelessParameter{13} => namelessParameter{13}.Dependencies.Model.FindEntityType("MountPointNtripGroup")];
                        instance2 = switch (entityType2)
                        {
                            case [LIFTABLE Constant: EntityType: MountPointNtripGroup (Dictionary<string, object>) CLR Type: Dictionary<string, object> | Resolver: namelessParameter{14} => namelessParameter{14}.Dependencies.Model.FindEntityType("MountPointNtripGroup")]:
                                {
                                    return
                                    {
                                        Dictionary<string, object> instance;
                                        instance = new Dictionary<string, object>();
                                        instance["AllowedGroupsId"] = dataReader.IsDBNull(22) ? default(object) : (object)dataReader.GetInt32(22);
                                        instance["MountPointsId"] = dataReader.IsDBNull(23) ? default(object) : (object)dataReader.GetInt32(23);
                                        (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                            context: materializationContext2.Context,
                                            entity: instance,
                                            queryTrackingBehavior: TrackAll,
                                            structuralType: [LIFTABLE Constant: EntityType: MountPointNtripGroup (Dictionary<string, object>) CLR Type: Dictionary<string, object> | Resolver: namelessParameter{15} => namelessParameter{15}.Dependencies.Model.FindEntityType("MountPointNtripGroup")]) : default(void);
                                        return instance;
                                    }}
                            default:
                                default(Dictionary<string, object>)
                        }
                        ;
                        entry2 = entityType2 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                            entityType: entityType2,
                            entity: instance2,
                            snapshot: shadowSnapshot2);
                        return instance2;
                    } : default(void);
                    return instance2;
                };
                entity =
                {
                    MaterializationContext materializationContext3;
                    IEntityType entityType3;
                    NtripGroup instance3;
                    InternalEntityEntry entry3;
                    bool hasNullKey3;
                    materializationContext3 = new MaterializationContext(
                        [LIFTABLE Constant: ValueBuffer | Resolver: _ => (object)ValueBuffer.Empty],
                        queryContext.Context
                    );
                    instance3 = default(NtripGroup);
                    entry3 = queryContext.TryGetEntry(
                        key: [LIFTABLE Constant: Key: NtripGroup.Id PK | Resolver: c => c.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup").FindPrimaryKey()],
                        keyValues: new object[]{ dataReader.IsDBNull(24) ? default(object) : (object)dataReader.GetInt32(24) },
                        throwOnNullKey: False,
                        hasNullKey: hasNullKey3);
                    !(hasNullKey3) ? entry3 != default(InternalEntityEntry) ?
                    {
                        entityType3 = entry3.EntityType;
                        return instance3 = (NtripGroup)entry3.Entity;
                    } :
                    {
                        ISnapshot shadowSnapshot3;
                        shadowSnapshot3 = [LIFTABLE Constant: Snapshot | Resolver: _ => Snapshot.Empty];
                        entityType3 = [LIFTABLE Constant: EntityType: NtripGroup | Resolver: namelessParameter{16} => namelessParameter{16}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup")];
                        instance3 = switch (entityType3)
                        {
                            case [LIFTABLE Constant: EntityType: NtripGroup | Resolver: namelessParameter{17} => namelessParameter{17}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup")]:
                                {
                                    return
                                    {
                                        NtripGroup instance;
                                        instance = new NtripGroup();
                                        instance.<Id>k__BackingField = dataReader.IsDBNull(24) ? default(int) : dataReader.GetInt32(24);
                                        instance.<CreatedAt>k__BackingField = dataReader.IsDBNull(25) ? default(DateTime) : dataReader.GetDateTime(25);
                                        instance.<Description>k__BackingField = dataReader.IsDBNull(26) ? default(string) : dataReader.GetString(26);
                                        instance.<IsActive>k__BackingField = dataReader.IsDBNull(27) ? default(bool) : dataReader.GetBoolean(27);
                                        instance.<Name>k__BackingField = dataReader.IsDBNull(28) ? default(string) : dataReader.GetString(28);
                                        (instance is IInjectableService) ? ((IInjectableService)instance).Injected(
                                            context: materializationContext3.Context,
                                            entity: instance,
                                            queryTrackingBehavior: TrackAll,
                                            structuralType: [LIFTABLE Constant: EntityType: NtripGroup | Resolver: namelessParameter{18} => namelessParameter{18}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup")]) : default(void);
                                        return instance;
                                    }}
                            default:
                                default(NtripGroup)
                        }
                        ;
                        entry3 = entityType3 == default(IEntityType) ? default(InternalEntityEntry) : queryContext.StartTracking(
                            entityType: entityType3,
                            entity: instance3,
                            snapshot: shadowSnapshot3);
                        return instance3;
                    } : default(void);
                    return instance3;
                };
                return NavigationExpandingExpressionVisitor.FetchJoinEntity<Dictionary<string, object>, NtripGroup>(
                    joinEntity: entity,
                    targetEntity: entity);
            },
            inverseNavigation: [LIFTABLE Constant: SkipNavigation: NtripGroup.MountPoints (ICollection<MountPoint>) CollectionMountPoint Inverse: AllowedGroups | Resolver: namelessParameter{19} => namelessParameter{19}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup").FindSkipNavigation("MountPoints")],
            fixup: (namelessParameter{20}, namelessParameter{21}) =>
            {
                [LIFTABLE Constant: ClrICollectionAccessor<MountPoint, ICollection<NtripGroup>, NtripGroup> | Resolver: namelessParameter{22} => namelessParameter{22}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.MountPoint").FindSkipNavigation("AllowedGroups").GetCollectionAccessor()].Add(
                    entity: namelessParameter{20},
                    value: namelessParameter{21},
                    forMaterialization: True);
                return [LIFTABLE Constant: ClrICollectionAccessor<NtripGroup, ICollection<MountPoint>, MountPoint> | Resolver: namelessParameter{23} => namelessParameter{23}.Dependencies.Model.FindEntityType("AgOpenNtripCaster.Server.Models.Entities.NtripGroup").FindSkipNavigation("MountPoints").GetCollectionAccessor()].Add(
                    entity: namelessParameter{21},
                    value: namelessParameter{20},
                    forMaterialization: True);
            },
            trackingQuery: True);
        return IsTrue(resultCoordinator.ResultReady)
         ? (MountPoint)(resultContext.Values[0]) : default(MountPoint);
    },
    contextType: AgOpenNtripCaster.Server.Data.ApplicationDbContext,
    standAloneStateManager: False,
    detailedErrorsEnabled: False,
    threadSafetyChecksEnabled: True)'
[23:10:41 DBG] Executing DbCommand [Parameters=[@__p_0='?' (DbType = Int32)], CommandType='Text', CommandTimeout='30']
SELECT a0."Id", a0."CreatedAt", a0."Description", a0."LogLevel", a0."MountPointId", a0."Type", a0."UserId", m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId", a1."Id", a1."AccessFailedCount", a1."ConcurrencyStamp", a1."CreatedAt", a1."Email", a1."EmailConfirmed", a1."FullName", a1."IsActive", a1."LastGeneratedSourcePassword", a1."LockoutEnabled", a1."LockoutEnd", a1."MaxConnections", a1."NormalizedEmail", a1."NormalizedUserName", a1."PasswordHash", a1."PhoneNumber", a1."PhoneNumberConfirmed", a1."RefreshToken", a1."RefreshTokenExpires", a1."SecurityStamp", a1."SourcePassword", a1."SourcePasswordGeneratedAt", a1."TwoFactorEnabled", a1."UserName"
FROM (
    SELECT a."Id", a."CreatedAt", a."Description", a."LogLevel", a."MountPointId", a."Type", a."UserId"
    FROM "Activities" AS a
    ORDER BY a."CreatedAt" DESC
    LIMIT @__p_0
) AS a0
LEFT JOIN "MountPoints" AS m ON a0."MountPointId" = m."Id"
LEFT JOIN "AspNetUsers" AS a1 ON a0."UserId" = a1."Id"
ORDER BY a0."CreatedAt" DESC
[23:10:41 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:41 DBG] Creating DbConnection.
[23:10:41 INF] Executed DbCommand (197ms) [Parameters=[@__p_0='?' (DbType = Int32)], CommandType='Text', CommandTimeout='30']
SELECT a0."Id", a0."CreatedAt", a0."Description", a0."LogLevel", a0."MountPointId", a0."Type", a0."UserId", m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId", a1."Id", a1."AccessFailedCount", a1."ConcurrencyStamp", a1."CreatedAt", a1."Email", a1."EmailConfirmed", a1."FullName", a1."IsActive", a1."LastGeneratedSourcePassword", a1."LockoutEnabled", a1."LockoutEnd", a1."MaxConnections", a1."NormalizedEmail", a1."NormalizedUserName", a1."PasswordHash", a1."PhoneNumber", a1."PhoneNumberConfirmed", a1."RefreshToken", a1."RefreshTokenExpires", a1."SecurityStamp", a1."SourcePassword", a1."SourcePasswordGeneratedAt", a1."TwoFactorEnabled", a1."UserName"
FROM (
    SELECT a."Id", a."CreatedAt", a."Description", a."LogLevel", a."MountPointId", a."Type", a."UserId"
    FROM "Activities" AS a
    ORDER BY a."CreatedAt" DESC
    LIMIT @__p_0
) AS a0
LEFT JOIN "MountPoints" AS m ON a0."MountPointId" = m."Id"
LEFT JOIN "AspNetUsers" AS a1 ON a0."UserId" = a1."Id"
ORDER BY a0."CreatedAt" DESC
[23:10:41 DBG] Created DbConnection. (7ms).
[23:10:42 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:42 DBG] Created DbCommand for 'ExecuteReader' (3ms).
[23:10:42 DBG] Initialized DbCommand for 'ExecuteReader' (10ms).
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status", m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId"
FROM "SourceConnections" AS s
INNER JOIN "MountPoints" AS m ON s."MountPointId" = m."Id"
WHERE s."DisconnectedAt" IS NULL
[23:10:42 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 INF] Executed DbCommand (31ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status", m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId"
FROM "SourceConnections" AS s
INNER JOIN "MountPoints" AS m ON s."MountPointId" = m."Id"
WHERE s."DisconnectedAt" IS NULL
[23:10:42 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:42 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Created DbCommand for 'ExecuteReader' (18ms).
[23:10:42 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 13ms reading results.
[23:10:42 DBG] Initialized DbCommand for 'ExecuteReader' (36ms).
[23:10:42 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Executing DbCommand [Parameters=[@__p_1='?' (DbType = Int32), @__p_0='?' (DbType = Int32)], CommandType='Text', CommandTimeout='30']
SELECT m0."Id", m0."BytesPerSecond", m0."CreatedAt", m0."Description", m0."DetectedFormat", m0."DetectedNavSystems", m0."Format", m0."IsActive", m0."LastRtcmMessageTime", m0."Latitude", m0."Longitude", m0."MaxClients", m0."MessageCount", m0."Name", m0."OwnerId", m0."ReferenceStationId", m0."RequireClientAuthentication", m0."RtcmLatitude", m0."RtcmLongitude", m0."SourcePassword", m0."UserId", a."Id", s."AllowedGroupsId", s."MountPointsId", s."Id", s."CreatedAt", s."Description", s."IsActive", s."Name", a."AccessFailedCount", a."ConcurrencyStamp", a."CreatedAt", a."Email", a."EmailConfirmed", a."FullName", a."IsActive", a."LastGeneratedSourcePassword", a."LockoutEnabled", a."LockoutEnd", a."MaxConnections", a."NormalizedEmail", a."NormalizedUserName", a."PasswordHash", a."PhoneNumber", a."PhoneNumberConfirmed", a."RefreshToken", a."RefreshTokenExpires", a."SecurityStamp", a."SourcePassword", a."SourcePasswordGeneratedAt", a."TwoFactorEnabled", a."UserName"
FROM (
    SELECT m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId"
    FROM "MountPoints" AS m
    LIMIT @__p_1 OFFSET @__p_0
) AS m0
LEFT JOIN "AspNetUsers" AS a ON m0."OwnerId" = a."Id"
LEFT JOIN (
    SELECT g."AllowedGroupsId", g."MountPointsId", n."Id", n."CreatedAt", n."Description", n."IsActive", n."Name"
    FROM "GroupMountPoints" AS g
    INNER JOIN "NtripGroups" AS n ON g."AllowedGroupsId" = n."Id"
) AS s ON m0."Id" = s."MountPointsId"
ORDER BY m0."Id", a."Id", s."AllowedGroupsId", s."MountPointsId"
[23:10:42 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (16ms).
[23:10:42 DBG] List of registered output formatters, in the following order: ["Microsoft.AspNetCore.Mvc.Formatters.HttpNoContentOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StringOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StreamOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter"]
[23:10:42 INF] Executed DbCommand (43ms) [Parameters=[@__p_1='?' (DbType = Int32), @__p_0='?' (DbType = Int32)], CommandType='Text', CommandTimeout='30']
SELECT m0."Id", m0."BytesPerSecond", m0."CreatedAt", m0."Description", m0."DetectedFormat", m0."DetectedNavSystems", m0."Format", m0."IsActive", m0."LastRtcmMessageTime", m0."Latitude", m0."Longitude", m0."MaxClients", m0."MessageCount", m0."Name", m0."OwnerId", m0."ReferenceStationId", m0."RequireClientAuthentication", m0."RtcmLatitude", m0."RtcmLongitude", m0."SourcePassword", m0."UserId", a."Id", s."AllowedGroupsId", s."MountPointsId", s."Id", s."CreatedAt", s."Description", s."IsActive", s."Name", a."AccessFailedCount", a."ConcurrencyStamp", a."CreatedAt", a."Email", a."EmailConfirmed", a."FullName", a."IsActive", a."LastGeneratedSourcePassword", a."LockoutEnabled", a."LockoutEnd", a."MaxConnections", a."NormalizedEmail", a."NormalizedUserName", a."PasswordHash", a."PhoneNumber", a."PhoneNumberConfirmed", a."RefreshToken", a."RefreshTokenExpires", a."SecurityStamp", a."SourcePassword", a."SourcePasswordGeneratedAt", a."TwoFactorEnabled", a."UserName"
FROM (
    SELECT m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId"
    FROM "MountPoints" AS m
    LIMIT @__p_1 OFFSET @__p_0
) AS m0
LEFT JOIN "AspNetUsers" AS a ON m0."OwnerId" = a."Id"
LEFT JOIN (
    SELECT g."AllowedGroupsId", g."MountPointsId", n."Id", n."CreatedAt", n."Description", n."IsActive", n."Name"
    FROM "GroupMountPoints" AS g
    INNER JOIN "NtripGroups" AS n ON g."AllowedGroupsId" = n."Id"
) AS s ON m0."Id" = s."MountPointsId"
ORDER BY m0."Id", a."Id", s."AllowedGroupsId", s."MountPointsId"
[23:10:42 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] No information found on request to perform content negotiation.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Attempting to select an output formatter without using a content type as no explicit content types were specified for the response.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Attempting to select the first formatter in the output formatters list which can write the result.
[23:10:42 DBG] Selected output formatter 'Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter' and content type 'application/json' to write the response.
[23:10:42 INF] Executing OkObjectResult, writing value of type 'AgOpenNtripCaster.Server.Models.DTOs.DashboardStatsDto'.
[23:10:42 INF] Executed action AgOpenNtripCaster.Server.Controllers.AdminConfigController.GetDashboardStats (AgOpenNtripCaster.Server) in 1116.6617ms
[23:10:42 INF] Request starting HTTP/1.1 GET http://localhost:5000/api/admin/config/stats - null null
[23:10:42 DBG] 1 candidate(s) found for the request path '/api/admin/config/stats'
[23:10:42 INF] Executed endpoint 'AgOpenNtripCaster.Server.Controllers.AdminConfigController.GetDashboardStats (AgOpenNtripCaster.Server)'
[23:10:42 DBG] Endpoint 'AgOpenNtripCaster.Server.Controllers.AdminConfigController.GetDashboardStats (AgOpenNtripCaster.Server)' with route pattern 'api/admin/config/stats' is valid for the request path '/api/admin/config/stats'
[23:10:42 INF] HTTP GET /api/admin/config/stats responded 200 in 1557.5397 ms
[23:10:42 DBG] Request matched endpoint 'AgOpenNtripCaster.Server.Controllers.AdminConfigController.GetDashboardStats (AgOpenNtripCaster.Server)'
[23:10:42 DBG] Connection id "0HNGR4N8NSOJ6" completed keep alive response.
[23:10:42 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:42 DBG] 'ApplicationDbContext' disposed.
[23:10:42 INF] CORS policy execution successful.
[23:10:42 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Successfully validated the token.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (5ms).
[23:10:42 DBG] AuthenticationScheme: Bearer was successfully authenticated.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'NtripUser' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 INF] Request finished HTTP/1.1 GET http://localhost:5000/api/admin/config/stats - 200 null application/json; charset=utf-8 1830.9819ms
[23:10:42 DBG] Authorization was successful.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 INF] Executing endpoint 'AgOpenNtripCaster.Server.Controllers.AdminConfigController.GetDashboardStats (AgOpenNtripCaster.Server)'
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 INF] Route matched with {action = "GetDashboardStats", controller = "AdminConfig"}. Executing controller action with signature System.Threading.Tasks.Task`1[Microsoft.AspNetCore.Mvc.ActionResult`1[AgOpenNtripCaster.Server.Models.DTOs.DashboardStatsDto]] GetDashboardStats() on controller AgOpenNtripCaster.Server.Controllers.AdminConfigController (AgOpenNtripCaster.Server).
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Execution plan of authorization filters (in the following order): ["None"]
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Execution plan of resource filters (in the following order): ["None"]
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Execution plan of action filters (in the following order): ["Microsoft.AspNetCore.Mvc.ModelBinding.UnsupportedContentTypeFilter (Order: -3000)", "Microsoft.AspNetCore.Mvc.Infrastructure.ModelStateInvalidFilter (Order: -2000)"]
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Execution plan of exception filters (in the following order): ["None"]
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Execution plan of result filters (in the following order): ["Microsoft.AspNetCore.Mvc.Infrastructure.ClientErrorResultFilter (Order: -2000)"]
[23:10:42 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Executing controller factory for controller AgOpenNtripCaster.Server.Controllers.AdminConfigController (AgOpenNtripCaster.Server)
[23:10:42 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 290ms reading results.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Executed controller factory for controller AgOpenNtripCaster.Server.Controllers.AdminConfigController (AgOpenNtripCaster.Server)
[23:10:42 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:42 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (11ms).
[23:10:42 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Creating DbConnection.
[23:10:42 DBG] Compiling query expression:
'DbSet<MountPoint>()
    .Count()'
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Created DbConnection. (11ms).
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Generated query execution expression:
'queryContext => ShapedQueryCompilingExpressionVisitor.SingleAsync<int>(
    asyncEnumerable: SingleQueryingEnumerable.Create<int>(
        relationalQueryContext: (RelationalQueryContext)queryContext,
        relationalCommandResolver: parameters => [LIFTABLE Constant: RelationalCommandCache.QueryExpression(
            Projection Mapping:
                EmptyProjectionMember -> 0
            SELECT CAST(count(*) AS integer)
            FROM MountPoints AS m) | Resolver: c => new RelationalCommandCache(
            c.Dependencies.MemoryCache,
            c.RelationalDependencies.QuerySqlGeneratorFactory,
            c.RelationalDependencies.RelationalParameterBasedSqlProcessorFactory,
            Projection Mapping:
                EmptyProjectionMember -> 0
            SELECT CAST(count(*) AS integer)
            FROM MountPoints AS m,
            False,
            new HashSet<string>(
                new string[]{ },
                StringComparer.Ordinal
            )
        )].GetRelationalCommandTemplate(parameters),
        readerColumns: null,
        shaper: (queryContext, dataReader, resultContext, resultCoordinator) =>
        {
            int? value1;
            value1 = dataReader.IsDBNull(0) ? default(int?) : (int?)dataReader.GetInt32(0);
            return (int)value1;
        },
        contextType: AgOpenNtripCaster.Server.Data.ApplicationDbContext,
        standAloneStateManager: False,
        detailedErrorsEnabled: False,
        threadSafetyChecksEnabled: True),
    cancellationToken: queryContext.CancellationToken)'
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:42 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Creating DbConnection.
[23:10:42 DBG] Created DbCommand for 'ExecuteReader' (47ms).
[23:10:42 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Created DbConnection. (18ms).
[23:10:42 DBG] Initialized DbCommand for 'ExecuteReader' (65ms).
[23:10:42 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:42 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 INF] Executed DbCommand (29ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:42 DBG] Created DbCommand for 'ExecuteReader' (27ms).
[23:10:42 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Initialized DbCommand for 'ExecuteReader' (46ms).
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:42 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 13ms reading results.
[23:10:42 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT count(*)::int
FROM "MountPoints" AS m
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Created DbCommand for 'ExecuteReader' (15ms).
[23:10:42 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 INF] Executed DbCommand (24ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT count(*)::int
FROM "MountPoints" AS m
[23:10:42 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Initialized DbCommand for 'ExecuteReader' (42ms).
[23:10:42 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (22ms).
[23:10:42 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:42 ERR] ?? STATS: ActiveClients count = 0
[23:10:42 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 22ms reading results.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 INF] Executed DbCommand (25ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT c."Id", c."BytesReceived", c."BytesSent", c."ClientIpAddress", c."ConnectedAt", c."DisconnectedAt", c."LastAccuracy", c."LastLatitude", c."LastLongitude", c."LastPositionAt", c."LastStreamPauseAt", c."MountPointId", c."SerialNumber", c."Status", c."UserId"
FROM "ClientSessions" AS c
WHERE c."DisconnectedAt" IS NULL
[23:10:42 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (13ms).
[23:10:42 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:42 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 15ms reading results.
[23:10:42 DBG] List of registered output formatters, in the following order: ["Microsoft.AspNetCore.Mvc.Formatters.HttpNoContentOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StringOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StreamOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter"]
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Created DbCommand for 'ExecuteReader' (17ms).
[23:10:42 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] No information found on request to perform content negotiation.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Initialized DbCommand for 'ExecuteReader' (37ms).
[23:10:42 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (21ms).
[23:10:42 DBG] Attempting to select an output formatter without using a content type as no explicit content types were specified for the response.
[23:10:42 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status", m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId"
FROM "SourceConnections" AS s
INNER JOIN "MountPoints" AS m ON s."MountPointId" = m."Id"
WHERE s."DisconnectedAt" IS NULL
[23:10:42 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Attempting to select the first formatter in the output formatters list which can write the result.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 INF] Executed DbCommand (23ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status", m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId"
FROM "SourceConnections" AS s
INNER JOIN "MountPoints" AS m ON s."MountPointId" = m."Id"
WHERE s."DisconnectedAt" IS NULL
[23:10:42 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Selected output formatter 'Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter' and content type 'application/json' to write the response.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:42 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:42 INF] Executing OkObjectResult, writing value of type 'AgOpenNtripCaster.Server.Models.DTOs.MountPointListResponse'.
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:42 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 26ms reading results.
[23:10:42 DBG] Created DbCommand for 'ExecuteReader' (22ms).
[23:10:42 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (16ms).
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Initialized DbCommand for 'ExecuteReader' (56ms).
[23:10:43 DBG] List of registered output formatters, in the following order: ["Microsoft.AspNetCore.Mvc.Formatters.HttpNoContentOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StringOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StreamOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter"]
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:10:43 DBG] No information found on request to perform content negotiation.
[23:10:43 INF] Executed action AgOpenNtripCaster.Server.Controllers.MountPointsController.GetMountPoints (AgOpenNtripCaster.Server) in 1886.143ms
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 INF] Request starting HTTP/1.1 GET http://localhost:5000/api/mountpoints?page=1&pageSize=100 - null null
[23:10:43 DBG] Attempting to select an output formatter without using a content type as no explicit content types were specified for the response.
[23:10:43 INF] Executed DbCommand (23ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT s."Id", s."BytesReceived", s."BytesSent", s."ConnectedAt", s."DisconnectedAt", s."MountPointId", s."Status"
FROM "SourceConnections" AS s
WHERE s."DisconnectedAt" IS NULL
[23:10:43 INF] Executed endpoint 'AgOpenNtripCaster.Server.Controllers.MountPointsController.GetMountPoints (AgOpenNtripCaster.Server)'
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] 1 candidate(s) found for the request path '/api/mountpoints'
[23:10:43 DBG] Attempting to select the first formatter in the output formatters list which can write the result.
[23:10:43 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 INF] HTTP GET /api/mountpoints responded 200 in 2268.5698 ms
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Endpoint 'AgOpenNtripCaster.Server.Controllers.MountPointsController.GetMountPoints (AgOpenNtripCaster.Server)' with route pattern 'api/MountPoints' is valid for the request path '/api/mountpoints'
[23:10:43 DBG] Selected output formatter 'Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter' and content type 'application/json' to write the response.
[23:10:43 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 19ms reading results.
[23:10:43 DBG] Connection id "0HNGR4N8NSOJ2" completed keep alive response.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Request matched endpoint 'AgOpenNtripCaster.Server.Controllers.MountPointsController.GetMountPoints (AgOpenNtripCaster.Server)'
[23:10:43 INF] Executing OkObjectResult, writing value of type 'AgOpenNtripCaster.Server.Models.DTOs.DashboardStatsDto'.
[23:10:43 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] 'ApplicationDbContext' disposed.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:43 INF] Executed action AgOpenNtripCaster.Server.Controllers.AdminConfigController.GetDashboardStats (AgOpenNtripCaster.Server) in 685.0594ms
[23:10:43 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (20ms).
[23:10:43 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (36ms).
[23:10:43 DBG] 'ApplicationDbContext' disposed.
[23:10:43 INF] CORS policy execution successful.
[23:10:43 INF] Executed endpoint 'AgOpenNtripCaster.Server.Controllers.AdminConfigController.GetDashboardStats (AgOpenNtripCaster.Server)'
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 INF] Request finished HTTP/1.1 GET http://localhost:5000/api/mountpoints?page=1&pageSize=100 - 200 null application/json; charset=utf-8 2556.5277ms
[23:10:43 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] Successfully validated the token.
[23:10:43 INF] HTTP GET /api/admin/config/stats responded 200 in 940.4754 ms
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (30ms).
[23:10:43 DBG] AuthenticationScheme: Bearer was successfully authenticated.
[23:10:43 DBG] Connection id "0HNGR4N8NSOJ1" completed keep alive response.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 INF] Executing endpoint 'AgOpenNtripCaster.Server.Controllers.MountPointsController.GetMountPoints (AgOpenNtripCaster.Server)'
[23:10:43 DBG] 'ApplicationDbContext' disposed.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 INF] Route matched with {action = "GetMountPoints", controller = "MountPoints"}. Executing controller action with signature System.Threading.Tasks.Task`1[Microsoft.AspNetCore.Mvc.ActionResult`1[AgOpenNtripCaster.Server.Models.DTOs.MountPointListResponse]] GetMountPoints(Int32, Int32) on controller AgOpenNtripCaster.Server.Controllers.MountPointsController (AgOpenNtripCaster.Server).
[23:10:43 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Execution plan of authorization filters (in the following order): ["None"]
[23:10:43 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (30ms).
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Execution plan of resource filters (in the following order): ["None"]
[23:10:43 INF] Request finished HTTP/1.1 GET http://localhost:5000/api/admin/config/stats - 200 null application/json; charset=utf-8 1113.0776ms
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Execution plan of action filters (in the following order): ["Microsoft.AspNetCore.Mvc.ModelBinding.UnsupportedContentTypeFilter (Order: -3000)", "Microsoft.AspNetCore.Mvc.Infrastructure.ModelStateInvalidFilter (Order: -2000)"]
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Execution plan of exception filters (in the following order): ["None"]
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Execution plan of result filters (in the following order): ["Microsoft.AspNetCore.Mvc.Infrastructure.ClientErrorResultFilter (Order: -2000)"]
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Executing controller factory for controller AgOpenNtripCaster.Server.Controllers.MountPointsController (AgOpenNtripCaster.Server)
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Executed controller factory for controller AgOpenNtripCaster.Server.Controllers.MountPointsController (AgOpenNtripCaster.Server)
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Attempting to bind parameter 'page' of type 'System.Int32' ...
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Attempting to bind parameter 'page' of type 'System.Int32' using the name 'page' in request data ...
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Done attempting to bind parameter 'page' of type 'System.Int32'.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Done attempting to bind parameter 'page' of type 'System.Int32'.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Attempting to validate the bound parameter 'page' of type 'System.Int32' ...
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Done attempting to validate the bound parameter 'page' of type 'System.Int32'.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Attempting to bind parameter 'pageSize' of type 'System.Int32' ...
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Attempting to bind parameter 'pageSize' of type 'System.Int32' using the name 'pageSize' in request data ...
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Done attempting to bind parameter 'pageSize' of type 'System.Int32'.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Done attempting to bind parameter 'pageSize' of type 'System.Int32'.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Attempting to validate the bound parameter 'pageSize' of type 'System.Int32' ...
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Done attempting to validate the bound parameter 'pageSize' of type 'System.Int32'.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Creating DbConnection.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Created DbConnection. (4ms).
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Created DbCommand for 'ExecuteReader' (7ms).
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Initialized DbCommand for 'ExecuteReader' (16ms).
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Executing DbCommand [Parameters=[@__p_1='?' (DbType = Int32), @__p_0='?' (DbType = Int32)], CommandType='Text', CommandTimeout='30']
SELECT m0."Id", m0."BytesPerSecond", m0."CreatedAt", m0."Description", m0."DetectedFormat", m0."DetectedNavSystems", m0."Format", m0."IsActive", m0."LastRtcmMessageTime", m0."Latitude", m0."Longitude", m0."MaxClients", m0."MessageCount", m0."Name", m0."OwnerId", m0."ReferenceStationId", m0."RequireClientAuthentication", m0."RtcmLatitude", m0."RtcmLongitude", m0."SourcePassword", m0."UserId", a."Id", s."AllowedGroupsId", s."MountPointsId", s."Id", s."CreatedAt", s."Description", s."IsActive", s."Name", a."AccessFailedCount", a."ConcurrencyStamp", a."CreatedAt", a."Email", a."EmailConfirmed", a."FullName", a."IsActive", a."LastGeneratedSourcePassword", a."LockoutEnabled", a."LockoutEnd", a."MaxConnections", a."NormalizedEmail", a."NormalizedUserName", a."PasswordHash", a."PhoneNumber", a."PhoneNumberConfirmed", a."RefreshToken", a."RefreshTokenExpires", a."SecurityStamp", a."SourcePassword", a."SourcePasswordGeneratedAt", a."TwoFactorEnabled", a."UserName"
FROM (
    SELECT m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId"
    FROM "MountPoints" AS m
    LIMIT @__p_1 OFFSET @__p_0
) AS m0
LEFT JOIN "AspNetUsers" AS a ON m0."OwnerId" = a."Id"
LEFT JOIN (
    SELECT g."AllowedGroupsId", g."MountPointsId", n."Id", n."CreatedAt", n."Description", n."IsActive", n."Name"
    FROM "GroupMountPoints" AS g
    INNER JOIN "NtripGroups" AS n ON g."AllowedGroupsId" = n."Id"
) AS s ON m0."Id" = s."MountPointsId"
ORDER BY m0."Id", a."Id", s."AllowedGroupsId", s."MountPointsId"
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 INF] Executed DbCommand (29ms) [Parameters=[@__p_1='?' (DbType = Int32), @__p_0='?' (DbType = Int32)], CommandType='Text', CommandTimeout='30']
SELECT m0."Id", m0."BytesPerSecond", m0."CreatedAt", m0."Description", m0."DetectedFormat", m0."DetectedNavSystems", m0."Format", m0."IsActive", m0."LastRtcmMessageTime", m0."Latitude", m0."Longitude", m0."MaxClients", m0."MessageCount", m0."Name", m0."OwnerId", m0."ReferenceStationId", m0."RequireClientAuthentication", m0."RtcmLatitude", m0."RtcmLongitude", m0."SourcePassword", m0."UserId", a."Id", s."AllowedGroupsId", s."MountPointsId", s."Id", s."CreatedAt", s."Description", s."IsActive", s."Name", a."AccessFailedCount", a."ConcurrencyStamp", a."CreatedAt", a."Email", a."EmailConfirmed", a."FullName", a."IsActive", a."LastGeneratedSourcePassword", a."LockoutEnabled", a."LockoutEnd", a."MaxConnections", a."NormalizedEmail", a."NormalizedUserName", a."PasswordHash", a."PhoneNumber", a."PhoneNumberConfirmed", a."RefreshToken", a."RefreshTokenExpires", a."SecurityStamp", a."SourcePassword", a."SourcePasswordGeneratedAt", a."TwoFactorEnabled", a."UserName"
FROM (
    SELECT m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId"
    FROM "MountPoints" AS m
    LIMIT @__p_1 OFFSET @__p_0
) AS m0
LEFT JOIN "AspNetUsers" AS a ON m0."OwnerId" = a."Id"
LEFT JOIN (
    SELECT g."AllowedGroupsId", g."MountPointsId", n."Id", n."CreatedAt", n."Description", n."IsActive", n."Name"
    FROM "GroupMountPoints" AS g
    INNER JOIN "NtripGroups" AS n ON g."AllowedGroupsId" = n."Id"
) AS s ON m0."Id" = s."MountPointsId"
ORDER BY m0."Id", a."Id", s."AllowedGroupsId", s."MountPointsId"
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:43 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 113ms reading results.
[23:10:43 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 1899ms reading results.
[23:10:43 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (8ms).
[23:10:43 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (5ms).
[23:10:43 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] List of registered output formatters, in the following order: ["Microsoft.AspNetCore.Mvc.Formatters.HttpNoContentOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StringOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StreamOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter"]
[23:10:43 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:43 DBG] No information found on request to perform content negotiation.
[23:10:43 DBG] Created DbCommand for 'ExecuteReader' (10ms).
[23:10:43 DBG] Initialized DbCommand for 'ExecuteReader' (17ms).
[23:10:43 DBG] Executing DbCommand [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT count(*)::int
FROM "MountPoints" AS m
[23:10:43 DBG] Attempting to select an output formatter without using a content type as no explicit content types were specified for the response.
[23:10:43 DBG] Attempting to select the first formatter in the output formatters list which can write the result.
[23:10:43 INF] Executed DbCommand (12ms) [Parameters=[], CommandType='Text', CommandTimeout='30']
SELECT count(*)::int
FROM "MountPoints" AS m
[23:10:43 DBG] Selected output formatter 'Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter' and content type 'application/json' to write the response.
[23:10:43 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:43 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 13ms reading results.
[23:10:43 INF] Executing OkObjectResult, writing value of type 'System.Collections.Generic.List`1[[AgOpenNtripCaster.Server.Services.NTRIP.ActivityDto, AgOpenNtripCaster.Server, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null]]'.
[23:10:44 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:44 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (13ms).
[23:10:44 INF] Executed action AgOpenNtripCaster.Server.Controllers.ActivityController.GetRecentActivities (AgOpenNtripCaster.Server) in 2915.0507ms
[23:10:44 INF] Request starting HTTP/1.1 GET http://localhost:5000/api/activity/recent?limit=50 - null null
[23:10:44 DBG] List of registered output formatters, in the following order: ["Microsoft.AspNetCore.Mvc.Formatters.HttpNoContentOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StringOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StreamOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter"]
[23:10:44 INF] Executed endpoint 'AgOpenNtripCaster.Server.Controllers.ActivityController.GetRecentActivities (AgOpenNtripCaster.Server)'
[23:10:44 DBG] 1 candidate(s) found for the request path '/api/activity/recent'
[23:10:44 DBG] No information found on request to perform content negotiation.
[23:10:44 INF] HTTP GET /api/activity/recent responded 200 in 3381.9327 ms
[23:10:44 DBG] Endpoint 'AgOpenNtripCaster.Server.Controllers.ActivityController.GetRecentActivities (AgOpenNtripCaster.Server)' with route pattern 'api/activity/recent' is valid for the request path '/api/activity/recent'
[23:10:44 DBG] Attempting to select an output formatter without using a content type as no explicit content types were specified for the response.
[23:10:44 DBG] Connection id "0HNGR4N8NSOJ5" completed keep alive response.
[23:10:44 DBG] Request matched endpoint 'AgOpenNtripCaster.Server.Controllers.ActivityController.GetRecentActivities (AgOpenNtripCaster.Server)'
[23:10:44 DBG] Attempting to select the first formatter in the output formatters list which can write the result.
[23:10:44 DBG] 'ApplicationDbContext' disposed.
[23:10:44 DBG] The request has an origin header: 'http://localhost:5173'.
[23:10:44 INF] CORS policy execution successful.
[23:10:44 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:44 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (9ms).
[23:10:44 DBG] Successfully validated the token.
[23:10:44 DBG] AuthenticationScheme: Bearer was successfully authenticated.
[23:10:44 INF] Request finished HTTP/1.1 GET http://localhost:5000/api/activity/recent?limit=50 - 200 null application/json; charset=utf-8 3628.7809ms
[23:10:44 DBG] Selected output formatter 'Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter' and content type 'application/json' to write the response.
[23:10:44 DBG] Authorization was successful.
[23:10:44 INF] Executing OkObjectResult, writing value of type 'AgOpenNtripCaster.Server.Models.DTOs.MountPointListResponse'.
[23:10:44 INF] Executing endpoint 'AgOpenNtripCaster.Server.Controllers.ActivityController.GetRecentActivities (AgOpenNtripCaster.Server)'
[23:10:44 INF] Executed action AgOpenNtripCaster.Server.Controllers.MountPointsController.GetMountPoints (AgOpenNtripCaster.Server) in 672.0285ms
[23:10:44 INF] Route matched with {action = "GetRecentActivities", controller = "Activity"}. Executing controller action with signature System.Threading.Tasks.Task`1[Microsoft.AspNetCore.Mvc.ActionResult`1[System.Collections.Generic.List`1[AgOpenNtripCaster.Server.Services.NTRIP.ActivityDto]]] GetRecentActivities(Int32) on controller AgOpenNtripCaster.Server.Controllers.ActivityController (AgOpenNtripCaster.Server).
[23:10:44 INF] Executed endpoint 'AgOpenNtripCaster.Server.Controllers.MountPointsController.GetMountPoints (AgOpenNtripCaster.Server)'
[23:10:44 DBG] Execution plan of authorization filters (in the following order): ["None"]
[23:10:44 INF] HTTP GET /api/mountpoints responded 200 in 1011.1694 ms
[23:10:44 DBG] Execution plan of resource filters (in the following order): ["None"]
[23:10:44 DBG] Connection id "0HNGR4N8NSOJ6" completed keep alive response.
[23:10:44 DBG] Execution plan of action filters (in the following order): ["Microsoft.AspNetCore.Mvc.ModelBinding.UnsupportedContentTypeFilter (Order: -3000)", "Microsoft.AspNetCore.Mvc.Infrastructure.ModelStateInvalidFilter (Order: -2000)"]
[23:10:44 DBG] 'ApplicationDbContext' disposed.
[23:10:44 DBG] Execution plan of exception filters (in the following order): ["None"]
[23:10:44 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:44 DBG] Execution plan of result filters (in the following order): ["Microsoft.AspNetCore.Mvc.Infrastructure.ClientErrorResultFilter (Order: -2000)"]
[23:10:44 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (10ms).
[23:10:44 DBG] Executing controller factory for controller AgOpenNtripCaster.Server.Controllers.ActivityController (AgOpenNtripCaster.Server)
[23:10:44 INF] Request finished HTTP/1.1 GET http://localhost:5000/api/mountpoints?page=1&pageSize=100 - 200 null application/json; charset=utf-8 1172.1108ms
[23:10:44 DBG] Executed controller factory for controller AgOpenNtripCaster.Server.Controllers.ActivityController (AgOpenNtripCaster.Server)
[23:10:44 DBG] Attempting to bind parameter 'limit' of type 'System.Int32' ...
[23:10:44 DBG] Attempting to bind parameter 'limit' of type 'System.Int32' using the name 'limit' in request data ...
[23:10:44 DBG] Done attempting to bind parameter 'limit' of type 'System.Int32'.
[23:10:44 DBG] Done attempting to bind parameter 'limit' of type 'System.Int32'.
[23:10:44 DBG] Attempting to validate the bound parameter 'limit' of type 'System.Int32' ...
[23:10:44 DBG] Done attempting to validate the bound parameter 'limit' of type 'System.Int32'.
[23:10:44 DBG] Entity Framework Core 9.0.10 initialized 'ApplicationDbContext' using provider 'Npgsql.EntityFrameworkCore.PostgreSQL:9.0.4+fd2380957bee5cd86f336466af36b08c0163f1a5' with options: None
[23:10:44 DBG] Creating DbConnection.
[23:10:44 DBG] Created DbConnection. (6ms).
[23:10:44 DBG] Opening connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:44 DBG] Opened connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:44 DBG] Creating DbCommand for 'ExecuteReader'.
[23:10:44 DBG] Created DbCommand for 'ExecuteReader' (2ms).
[23:10:44 DBG] Initialized DbCommand for 'ExecuteReader' (6ms).
[23:10:44 DBG] Executing DbCommand [Parameters=[@__p_0='?' (DbType = Int32)], CommandType='Text', CommandTimeout='30']
SELECT a0."Id", a0."CreatedAt", a0."Description", a0."LogLevel", a0."MountPointId", a0."Type", a0."UserId", m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId", a1."Id", a1."AccessFailedCount", a1."ConcurrencyStamp", a1."CreatedAt", a1."Email", a1."EmailConfirmed", a1."FullName", a1."IsActive", a1."LastGeneratedSourcePassword", a1."LockoutEnabled", a1."LockoutEnd", a1."MaxConnections", a1."NormalizedEmail", a1."NormalizedUserName", a1."PasswordHash", a1."PhoneNumber", a1."PhoneNumberConfirmed", a1."RefreshToken", a1."RefreshTokenExpires", a1."SecurityStamp", a1."SourcePassword", a1."SourcePasswordGeneratedAt", a1."TwoFactorEnabled", a1."UserName"
FROM (
    SELECT a."Id", a."CreatedAt", a."Description", a."LogLevel", a."MountPointId", a."Type", a."UserId"
    FROM "Activities" AS a
    ORDER BY a."CreatedAt" DESC
    LIMIT @__p_0
) AS a0
LEFT JOIN "MountPoints" AS m ON a0."MountPointId" = m."Id"
LEFT JOIN "AspNetUsers" AS a1 ON a0."UserId" = a1."Id"
ORDER BY a0."CreatedAt" DESC
[23:10:44 INF] Executed DbCommand (19ms) [Parameters=[@__p_0='?' (DbType = Int32)], CommandType='Text', CommandTimeout='30']
SELECT a0."Id", a0."CreatedAt", a0."Description", a0."LogLevel", a0."MountPointId", a0."Type", a0."UserId", m."Id", m."BytesPerSecond", m."CreatedAt", m."Description", m."DetectedFormat", m."DetectedNavSystems", m."Format", m."IsActive", m."LastRtcmMessageTime", m."Latitude", m."Longitude", m."MaxClients", m."MessageCount", m."Name", m."OwnerId", m."ReferenceStationId", m."RequireClientAuthentication", m."RtcmLatitude", m."RtcmLongitude", m."SourcePassword", m."UserId", a1."Id", a1."AccessFailedCount", a1."ConcurrencyStamp", a1."CreatedAt", a1."Email", a1."EmailConfirmed", a1."FullName", a1."IsActive", a1."LastGeneratedSourcePassword", a1."LockoutEnabled", a1."LockoutEnd", a1."MaxConnections", a1."NormalizedEmail", a1."NormalizedUserName", a1."PasswordHash", a1."PhoneNumber", a1."PhoneNumberConfirmed", a1."RefreshToken", a1."RefreshTokenExpires", a1."SecurityStamp", a1."SourcePassword", a1."SourcePasswordGeneratedAt", a1."TwoFactorEnabled", a1."UserName"
FROM (
    SELECT a."Id", a."CreatedAt", a."Description", a."LogLevel", a."MountPointId", a."Type", a."UserId"
    FROM "Activities" AS a
    ORDER BY a."CreatedAt" DESC
    LIMIT @__p_0
) AS a0
LEFT JOIN "MountPoints" AS m ON a0."MountPointId" = m."Id"
LEFT JOIN "AspNetUsers" AS a1 ON a0."UserId" = a1."Id"
ORDER BY a0."CreatedAt" DESC
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'NtripUser' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.MountPoint' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'MountPoint' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] The navigation 'Activity.User' was detected as changed. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Context 'ApplicationDbContext' started tracking 'Activity' entity. Consider using 'DbContextOptionsBuilder.EnableSensitiveDataLogging' to see key values.
[23:10:44 DBG] Closing data reader to 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:44 DBG] A data reader for 'ntripcaster' on server 'tcp://192.168.2.205:5433' is being disposed after spending 587ms reading results.
[23:10:44 DBG] Closing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:44 DBG] Closed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (2ms).
[23:10:44 DBG] List of registered output formatters, in the following order: ["Microsoft.AspNetCore.Mvc.Formatters.HttpNoContentOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StringOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.StreamOutputFormatter", "Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter"]
[23:10:44 DBG] No information found on request to perform content negotiation.
[23:10:44 DBG] Attempting to select an output formatter without using a content type as no explicit content types were specified for the response.
[23:10:44 DBG] Attempting to select the first formatter in the output formatters list which can write the result.
[23:10:44 DBG] Selected output formatter 'Microsoft.AspNetCore.Mvc.Formatters.SystemTextJsonOutputFormatter' and content type 'application/json' to write the response.
[23:10:45 INF] Executing OkObjectResult, writing value of type 'System.Collections.Generic.List`1[[AgOpenNtripCaster.Server.Services.NTRIP.ActivityDto, AgOpenNtripCaster.Server, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null]]'.
[23:10:45 INF] Executed action AgOpenNtripCaster.Server.Controllers.ActivityController.GetRecentActivities (AgOpenNtripCaster.Server) in 788.6682ms
[23:10:45 INF] Executed endpoint 'AgOpenNtripCaster.Server.Controllers.ActivityController.GetRecentActivities (AgOpenNtripCaster.Server)'
[23:10:45 INF] HTTP GET /api/activity/recent responded 200 in 917.5257 ms
[23:10:45 DBG] Connection id "0HNGR4N8NSOJ1" completed keep alive response.
[23:10:45 DBG] 'ApplicationDbContext' disposed.
[23:10:45 DBG] Disposing connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433'.
[23:10:45 DBG] Disposed connection to database 'ntripcaster' on server 'tcp://192.168.2.205:5433' (7ms).
[23:10:45 INF] Request finished HTTP/1.1 GET http://localhost:5000/api/activity/recent?limit=50 - 200 null application/json; charset=utf-8 1012.3658ms
