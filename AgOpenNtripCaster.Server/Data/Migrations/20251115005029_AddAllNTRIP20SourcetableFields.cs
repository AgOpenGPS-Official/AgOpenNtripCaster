using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace AgOpenNtripCaster.Server.Data.Migrations
{
    /// <inheritdoc />
    public partial class AddAllNTRIP20SourcetableFields : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<string>(
                name: "Authentication",
                table: "MountPoints",
                type: "text",
                nullable: false,
                defaultValue: "");

            migrationBuilder.AddColumn<int>(
                name: "Carrier",
                table: "MountPoints",
                type: "integer",
                nullable: false,
                defaultValue: 0);

            migrationBuilder.AddColumn<string>(
                name: "Compression",
                table: "MountPoints",
                type: "text",
                nullable: false,
                defaultValue: "");

            migrationBuilder.AddColumn<string>(
                name: "Country",
                table: "MountPoints",
                type: "text",
                nullable: false,
                defaultValue: "");

            migrationBuilder.AddColumn<bool>(
                name: "FeeRequired",
                table: "MountPoints",
                type: "boolean",
                nullable: false,
                defaultValue: false);

            migrationBuilder.AddColumn<string>(
                name: "Generator",
                table: "MountPoints",
                type: "text",
                nullable: false,
                defaultValue: "");

            migrationBuilder.AddColumn<string>(
                name: "Misc",
                table: "MountPoints",
                type: "text",
                nullable: true);

            migrationBuilder.AddColumn<string>(
                name: "NavSystem",
                table: "MountPoints",
                type: "text",
                nullable: true);

            migrationBuilder.AddColumn<string>(
                name: "Network",
                table: "MountPoints",
                type: "text",
                nullable: false,
                defaultValue: "");

            migrationBuilder.AddColumn<bool>(
                name: "NmeaRequired",
                table: "MountPoints",
                type: "boolean",
                nullable: false,
                defaultValue: false);

            migrationBuilder.AddColumn<int>(
                name: "Solution",
                table: "MountPoints",
                type: "integer",
                nullable: false,
                defaultValue: 0);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "Authentication",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "Carrier",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "Compression",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "Country",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "FeeRequired",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "Generator",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "Misc",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "NavSystem",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "Network",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "NmeaRequired",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "Solution",
                table: "MountPoints");
        }
    }
}
